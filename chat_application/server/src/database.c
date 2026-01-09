#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "database.h"
#include "network.h"
#include "globals.h"

PGconn *conn = NULL;
pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;
static DBConfig default_config = {
    .host = "localhost",
    .port = "5432",
    .dbname = "chat_db",
    .user = "",  
    .password = ""
};

// Trim whitespace from string
static void trim(char *str) {
    char *end;
    
    // Trim leading space
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0) return;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

int db_load_config(const char *config_file, DBConfig *config) {
    FILE *file = fopen(config_file, "r");
    if (!file) {
        fprintf(stderr, "[DB] Config file '%s' not found, using defaults\n", config_file);
        *config = default_config;
        return -1;
    }

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        char key[128], value[256];
        if (sscanf(line, "%127[^=]=%255[^\n]", key, value) == 2) {
            trim(key);
            trim(value);
            if (strcmp(key, "DB_HOST") == 0) {
                strncpy(config->host, value, sizeof(config->host) - 1);
            } else if (strcmp(key, "DB_PORT") == 0) {
                strncpy(config->port, value, sizeof(config->port) - 1);
            } else if (strcmp(key, "DB_NAME") == 0) {
                strncpy(config->dbname, value, sizeof(config->dbname) - 1);
            } else if (strcmp(key, "DB_USER") == 0) {
                strncpy(config->user, value, sizeof(config->user) - 1);
            } else if (strcmp(key, "DB_PASSWORD") == 0) {
                strncpy(config->password, value, sizeof(config->password) - 1);
            }
        }
    }
    fclose(file);
    printf("[DB] Configuration loaded from '%s'\n", config_file);
    return 0;
}

void db_connect_with_config(DBConfig *config) {
    char conninfo[1024];
    
    // Build connection string
    if (strlen(config->user) > 0 && strlen(config->password) > 0) {
        // With username and password
        snprintf(conninfo, sizeof(conninfo),
                 "host=%s port=%s dbname=%s user=%s password=%s",
                 config->host, config->port, config->dbname,
                 config->user, config->password);
    } else if (strlen(config->user) > 0) {
        // With username only (peer auth or trust)
        snprintf(conninfo, sizeof(conninfo),
                 "host=%s port=%s dbname=%s user=%s",
                 config->host, config->port, config->dbname, config->user);
    } else {
        // No credentials - use system user (peer authentication)
        snprintf(conninfo, sizeof(conninfo),
                 "host=%s port=%s dbname=%s",
                 config->host, config->port, config->dbname);
    }
    
    printf("[DB] Connecting to: host=%s port=%s dbname=%s user=%s\n",
           config->host, config->port, config->dbname,
           strlen(config->user) > 0 ? config->user : "(system user)");
    
    printf("[DB] Connection string: %s\n", conninfo);
    
    conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Failed to connect Database: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    printf("Connected Database successfully!\n");
}

PGresult *db_exec(const char *query) {
    PGresult *res = NULL;
    pthread_mutex_lock(&db_mutex);
    res = PQexec(conn, query);
    pthread_mutex_unlock(&db_mutex);
    return res;
}

PGresult *db_exec_params(const char *query, int nParams, const char *const *paramValues) {
    PGresult *res = NULL;
    pthread_mutex_lock(&db_mutex);
    res = PQexecParams(conn, query, nParams, NULL, paramValues, NULL, NULL, 0);
    pthread_mutex_unlock(&db_mutex);
    return res;
}

void db_connect() {
    DBConfig config;
    const char *cfg = getenv("DB_CONFIG");
    if (!cfg) cfg = "database/db.conf";
    if (db_load_config(cfg, &config) != 0) {
        config = default_config;
    }
    db_connect_with_config(&config);
}

void db_disconnect() {
    if (conn != NULL) {
        PQfinish(conn);
        printf("Closed Database connection.\n");
    }
}

void db_reset_all_online_status() {
    const char *query = "UPDATE \"User\" SET status = '0';";
    PGresult *res = db_exec(query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "[DB] Reset online status failed: %s\n", PQerrorMessage(conn));
    } else {
        printf("[DB] All users reset to offline status.\n");
    }
    PQclear(res);
}

void db_save_message(const char *sender, const char *receiver, const char *content, int delivered) {
    const char *query = "INSERT INTO \"MessageLog\" (sender, receiver, \"content\", sentTime, is_delivered) "
                        "VALUES ($1, $2, $3, CURRENT_TIMESTAMP, $4);";
    const char *params[4] = {sender, receiver, content, delivered ? "TRUE" : "FALSE"};
    PGresult *res = db_exec_params(query, 4, params);
    PQclear(res);
}

void db_get_history_and_mark_read(int client_fd, const char *me, const char *friend) {
    const char *query = "SELECT sender, \"content\", sentTime FROM \"MessageLog\" "
                        "WHERE (sender = $1 AND receiver = $2) OR (sender = $2 AND receiver = $1) "
                        "ORDER BY messageId ASC LIMIT 50;";
    const char *params[2] = {me, friend};
    PGresult *res = db_exec_params(query, 2, params);

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        send_line(client_fd, "--- CHAT HISTORY ---\n");
        for (int i = 0; i < PQntuples(res); i++) {
            char buf[BUF_SIZE];
            snprintf(buf, sizeof(buf), "[%s] %s: %s\n", PQgetvalue(res, i, 2), PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
            send_line(client_fd, buf);
        }
        send_line(client_fd, "--------------------\n");
        
        // Update đã đọc
        const char *update_q = "UPDATE \"MessageLog\" SET is_delivered = TRUE WHERE sender = $1 AND receiver = $2;";
        const char *update_params[2] = {friend, me};
        PGresult *up_res = db_exec_params(update_q, 2, update_params);
        PQclear(up_res);
    }
    PQclear(res);
}

void db_notify_pending(int client_fd, const char *me) {
    const char *q = "SELECT DISTINCT sender FROM \"MessageLog\" WHERE receiver = $1 AND is_delivered = FALSE;";
    const char *params[1] = {me};
    PGresult *res = db_exec_params(q, 1, params);
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        char list[BUF_SIZE] = "NOTIFICATION: Users have sent you messages: ";
        for (int i = 0; i < PQntuples(res); i++) {
            strcat(list, PQgetvalue(res, i, 0));
            if (i < PQntuples(res)-1) strcat(list, ", ");
        }
        strcat(list, ". Press CHAT <username> to see messages.\n");
        send_line(client_fd, list);
    }
    PQclear(res);
}
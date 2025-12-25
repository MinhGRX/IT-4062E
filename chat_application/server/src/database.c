#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "network.h"
#include "globals.h"

PGconn *conn = NULL; 

void db_connect() {
    const char *conninfo = "host=localhost port=5432 dbname=chat_application_2 user=postgres password=123456";
    
    conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Kết nối Database thất bại: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    printf("Kết nối Database thành công!\n");
}

void db_disconnect() {
    if (conn != NULL) {
        PQfinish(conn);
        printf("Đã đóng kết nối Database.\n");
    }
}

void db_reset_all_online_status() {
    const char *query = "UPDATE \"User\" SET status = '0';";
    PGresult *res = PQexec(conn, query);

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
    PGresult *res = PQexecParams(conn, query, 4, NULL, params, NULL, NULL, 0);
    PQclear(res);
}

void db_get_history_and_mark_read(int client_fd, const char *me, const char *friend) {
    const char *query = "SELECT sender, \"content\", sentTime FROM \"MessageLog\" "
                        "WHERE (sender = $1 AND receiver = $2) OR (sender = $2 AND receiver = $1) "
                        "ORDER BY messageId ASC LIMIT 50;";
    const char *params[2] = {me, friend};
    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        send_line(client_fd, "--- CHAT HISTORY ---\n");
        for (int i = 0; i < PQntuples(res); i++) {
            char buf[BUF_SIZE];
            snprintf(buf, sizeof(buf), "[%s] %s: %s\n", PQgetvalue(res, i, 2), PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
            send_line(client_fd, buf);
        }
        send_line(client_fd, "--------------------\n");
        
        // Update đã đọc
        char up_q[256];
        sprintf(up_q, "UPDATE \"MessageLog\" SET is_delivered = TRUE WHERE sender = '%s' AND receiver = '%s';", friend, me);
        PQexec(conn, up_q);
    }
    PQclear(res);
}

void db_notify_pending(int client_fd, const char *me) {
    const char *q = "SELECT DISTINCT sender FROM \"MessageLog\" WHERE receiver = $1 AND is_delivered = FALSE;";
    const char *params[1] = {me};
    PGresult *res = PQexecParams(conn, q, 1, NULL, params, NULL, NULL, 0);
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        char list[BUF_SIZE] = "THÔNG BÁO: Các user đã nhắn cho bạn: ";
        for (int i = 0; i < PQntuples(res); i++) {
            strcat(list, PQgetvalue(res, i, 0));
            if (i < PQntuples(res)-1) strcat(list, ", ");
        }
        strcat(list, ". Gõ CHAT <tên> để xem.\n");
        send_line(client_fd, list);
    }
    PQclear(res);
}
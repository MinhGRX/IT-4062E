#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "../../include/dao/group_dao.h"
#include "../../include/network.h"
#include <globals.h>

int group_dao_create(const char *group_name, const char *creator, int *out_group_id) {
    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO \"Group\" (\"groupName\", \"creator\") VALUES ('%s', '%s') RETURNING \"groupId\";",
        group_name, creator);
    
    PGresult *res = PQexec(conn, query);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SQL Error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    
    *out_group_id = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    
    // Add creator as owner
    return group_dao_add_member(*out_group_id, creator, "owner");
}

int group_dao_add_member(int group_id, const char *username, const char *role) {
    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO \"GroupMember\" (\"groupId\", \"username\", \"role\") VALUES (%d, '%s', '%s') ON CONFLICT DO NOTHING;",
        group_id, username, role);
    
    PGresult *res = PQexec(conn, query);
    
    int ok = PQresultStatus(res) == PGRES_COMMAND_OK;
    PQclear(res);
    return ok ? 0 : -1;
}

int group_dao_is_member(int group_id, const char *username) {
    if (!username || strlen(username) == 0) {
        fprintf(stderr, "ERROR: Invalid username\n");
        return 0;
    }
    
    char query[512];
    snprintf(query, sizeof(query),
        "SELECT 1 FROM \"GroupMember\" WHERE \"groupId\" = %d AND \"username\" = '%s';",
        group_id, username);
    
    PGresult *res = PQexec(conn, query);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SQL Error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }
    
    int is_member = PQntuples(res) > 0 ? 1 : 0;
    PQclear(res);
    
    return is_member;
}

int group_dao_is_owner(int group_id, const char *username) {
    if (!username || strlen(username) == 0) {
        fprintf(stderr, "ERROR: Invalid username\n");
        return 0;
    }
    
    char query[512];
    snprintf(query, sizeof(query),
        "SELECT 1 FROM \"GroupMember\" "
        "WHERE \"groupId\" = %d AND \"username\" = '%s' AND \"role\" = 'owner';",
        group_id, username);
    
    PGresult *res = PQexec(conn, query);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SQL Error in group_dao_is_owner: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }
    
    int is_owner = PQntuples(res) > 0 ? 1 : 0;
    PQclear(res);
    
    return is_owner;
}

int group_dao_remove_member(int group_id, const char *username) {
    if (!username || strlen(username) == 0) {
        fprintf(stderr, "ERROR: Invalid username\n");
        return -1;
    }
    
    char query[512];
    snprintf(query, sizeof(query),
        "DELETE FROM \"GroupMember\" "
        "WHERE \"groupId\" = %d AND \"username\" = '%s';",
        group_id, username);
    
    PGresult *res = PQexec(conn, query);
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "SQL Error in group_dao_remove_member: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    
    PQclear(res);
    return 0;
}
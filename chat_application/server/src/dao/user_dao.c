#include "dao/user_dao.h"
#include <stdlib.h>
#include <string.h>

int user_dao_exists(const char *username) {
    const char *params[1] = {username};
    PGresult *res = PQexecParams(conn, "SELECT 1 FROM \"User\" WHERE username = $1", 1, NULL, params, NULL, NULL, 0);
    
    int exists = (PQntuples(res) > 0);
    PQclear(res);
    return exists;
}

int user_dao_create(const char *username, const char *password) {
    const char *params[2] = {username, password};
    PGresult *res = PQexecParams(conn, "INSERT INTO \"User\" (username, password, status) VALUES ($1, $2, '0')", 2, NULL, params, NULL, NULL, 0);
    
    int success = (PQresultStatus(res) == PGRES_COMMAND_OK);
    PQclear(res);
    return success;
}

int user_dao_authenticate(const char *username, const char *password) {
    const char *params[2] = {username, password};
    PGresult *res = PQexecParams(conn, "SELECT 1 FROM \"User\" WHERE username = $1 AND password = $2", 2, NULL, params, NULL, NULL, 0);
    
    int auth = (PQntuples(res) > 0);
    PQclear(res);
    return auth;
}
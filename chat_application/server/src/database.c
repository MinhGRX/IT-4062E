#include "database.h"
#include <stdio.h>
#include <stdlib.h>

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
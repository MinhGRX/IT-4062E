#ifndef DATABASE_H
#define DATABASE_H
#include <libpq-fe.h>

extern PGconn *conn;
void db_connect();
void db_disconnect();
void db_reset_all_online_status();
void db_save_message();
void db_get_history_and_mark_read();
void db_notify_pending();

#endif
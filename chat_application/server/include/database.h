#ifndef DATABASE_H
#define DATABASE_H

#include <libpq-fe.h>
#include <pthread.h>

#define BUF_SIZE 4096

extern PGconn *conn;
extern pthread_mutex_t db_mutex;

typedef struct {
  char host[256];
  char port[16];
  char dbname[256];
  char user[256];
  char password[256];
} DBConfig;

// Load database configuration from file
int db_load_config(const char *config_file, DBConfig *config);

// Connect to database using config
void db_connect();
void db_connect_with_config(DBConfig *config);

void db_disconnect();
void db_reset_all_online_status();
void db_save_message(const char *sender, const char *receiver,
                     const char *content, int delivered);
void db_get_history_and_mark_read(int client_fd, const char *me,
                                  const char *friend);
void db_notify_pending(int client_fd, const char *me);

// Thread-safe execution helpers
PGresult *db_exec(const char *query);
PGresult *db_exec_params(const char *query, int nParams,
                         const char *const *paramValues);

#endif
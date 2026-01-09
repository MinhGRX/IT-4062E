#ifndef CHAT_DAO_H
#define CHAT_DAO_H

#include "../database.h"
#include <libpq-fe.h>


void chat_dao_save_message(const char *sender, const char *receiver,
                           const char *content, int delivered);
void chat_dao_get_history(int client_fd, const char *me,
                          const char *friend_name);
void chat_dao_mark_delivered(const char *me, const char *friend_name);
void chat_dao_get_pending_senders(int client_fd, const char *me);

#endif
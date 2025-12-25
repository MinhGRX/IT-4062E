#ifndef USER_DAO_H
#define USER_DAO_H

#include "database.h"

int user_dao_exists(const char *username);
int user_dao_create(const char *username, const char *password);
int user_dao_authenticate(const char *username, const char *password);
void user_dao_update_status(const char *username, const char *status);

#endif
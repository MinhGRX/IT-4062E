#include "services/user_service.h"
#include "dao/user_dao.h"
#include <string.h>

int user_service_register(const char *username, const char *password)
{
    if (strlen(password) < 6)
        return -2;
    if (user_dao_exists(username))
        return -1;

    return user_dao_create(username, password) ? 1 : 0;
}

int user_service_login(const char *username, const char *password)
{
    return user_dao_authenticate(username, password);
}
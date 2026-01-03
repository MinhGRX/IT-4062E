#ifndef GROUP_CONTROLLER_H
#define GROUP_CONTROLLER_H

#include <libpq-fe.h>
#include "../globals.h"

void group_controller_create(int client_fd, char *username, const char *group_name);

#endif
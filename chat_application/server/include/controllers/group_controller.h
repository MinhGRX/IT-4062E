#ifndef GROUP_CONTROLLER_H
#define GROUP_CONTROLLER_H

#include <libpq-fe.h>
#include "../globals.h"

// Create a new group
void group_controller_create(int client_fd, char *username, const char *group_name);

// Add member to group (any member can add)
void group_controller_add_member(int client_fd, int group_id, const char *requester, const char *username_to_add);

#endif
#ifndef GROUP_DAO_H
#define GROUP_DAO_H

#include <libpq-fe.h>
#include "../database.h"

// Create a new group
int group_dao_create(const char *group_name, const char *creator, int *out_group_id);

// Add a member to a group
int group_dao_add_member(int group_id, const char *username, const char *role);

// Check if user is group member
int group_dao_is_member(int group_id, const char *username);


#endif
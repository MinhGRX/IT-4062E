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

// Check if user is group owner
int group_dao_is_owner(int group_id, const char *username);

// Remove a member from a group
int group_dao_remove_member(int group_id, const char *username);

// Save group message to database
int group_dao_save_message(int group_id, const char *sender, const char *message);

// Get group chat history (last N messages)
int group_dao_get_history(int group_id, int limit, char ***out_messages, int *out_count);

// Get all members of a group
int group_dao_get_members(int group_id, char ***out_members, int *out_count);

// List all groups a user belongs to
int group_dao_list_user_groups(const char *username, char ***out_groups, int *out_count);

#endif
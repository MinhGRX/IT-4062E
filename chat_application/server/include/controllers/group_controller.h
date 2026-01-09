#ifndef GROUP_CONTROLLER_H
#define GROUP_CONTROLLER_H

#include "../globals.h"
#include <libpq-fe.h>


// Create a new group
void group_controller_create(int client_fd, char *username,
                             const char *group_name);

// Add member to group (any member can add)
void group_controller_add_member(int client_fd, const char *username,
                                 const char *group_id_str,
                                 const char *username_to_add);

// Remove member from group (owner only)
void group_controller_remove_member(int client_fd, const char *username,
                                    const char *group_id_str,
                                    const char *username_to_remove);

// Leave group (any member except owner)
void group_controller_leave(int client_fd, const char *username,
                            const char *group_id_str);

// Send message to group
void group_controller_send_message(int client_fd, const char *username,
                                   const char *group_id_str,
                                   const char *message);

// Get group chat history
void group_controller_get_history(int client_fd, const char *username,
                                  const char *group_id_str);

// List all members of a group
void group_controller_get_members(int client_fd, const char *username,
                                  const char *group_id_str);

#endif
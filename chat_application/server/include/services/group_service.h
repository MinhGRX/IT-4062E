#ifndef GROUP_SERVICE_H
#define GROUP_SERVICE_H

// Validate and create group
int group_service_create(int client_fd, const char *username, const char *group_name);

// Validate and add member to group
int group_service_add_member(int client_fd, const char *username, int group_id, const char *username_to_add);

// Validate and remove member from group
int group_service_remove_member(int client_fd, const char *username, int group_id, const char *username_to_remove);

// Leave group
int group_service_leave_group(int client_fd, const char *username, int group_id);

// Validate and send message to group
int group_service_send_message(int client_fd, const char *username, int group_id, const char *message);

// View group chat history
int group_service_get_history(int client_fd, const char *username,  int group_id, int limit, const char ***messages, int *msg_count);

// View group members
int group_service_get_members(int client_fd, const char *username, int group_id);

// List groups for a user
int group_service_list_groups(int client_fd, const char *username);
#endif

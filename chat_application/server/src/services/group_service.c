#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "services/group_service.h"
#include "globals.h"
#include "services/log_service.h"
#include "dao/group_dao.h"
#include "dao/friend_dao.h"
#include "network.h"

int group_service_create(int client_fd, const char *username, const char *group_name)
{
    // Validation at service layer
    if (!username || !group_name || strlen(username) == 0 || strlen(group_name) == 0)
    {
        send_line(client_fd, "ERR Invalid username or group name\n");
        log_activity("[ERROR] GROUP_CREATE: Invalid input from user");
        return -1;
    }

    // Call DAO to do actual work
    int group_id = 0;
    if (group_dao_create(group_name, username, &group_id) != 0)
    {
        send_line(client_fd, "ERR Failed to create group (name may exist)\n");
        log_activity("[ERROR] GROUP_CREATE: Failed to create group (name may exist)");
        return -1;
    }

    char ok[128];
    snprintf(ok, sizeof(ok), "OK Group created successfully (ID:%d)\n", group_id);
    send_line(client_fd, ok);
    log_activity("[INFO] GROUP_CREATE: User %s created group %d (%s)", username, group_id, group_name);
    return 0;
}

int group_service_add_member(int client_fd, const char *username, int group_id, const char *username_to_add)
{
    // Input validation at service layer
    if (!username || !username_to_add || strlen(username) == 0 || strlen(username_to_add) == 0 || group_id <= 0)
    {
        send_line(client_fd, "ERR Invalid parameters\n");
        log_activity("[ERROR] GROUP_ADD_MEMBER: Invalid input parameters");
        return -1;
    }

    // check if caller is member
    if (!group_dao_is_member(group_id, username))
    {
        send_line(client_fd, "ERR You must be a member of this group to add others\n");
        log_activity("[ERROR] GROUP_ADD_MEMBER: User %s not member of group %d", username, group_id);
        return -1;
    }

    // check friendship
    if (!friend_dao_are_friends(username, username_to_add))
    {
        send_line(client_fd, "ERR You can only add friends to the group\n");
        log_activity("[ERROR] GROUP_ADD_MEMBER: Users %s and %s are not friends", username, username_to_add);
        return -1;
    }

    // check if already member
    if (group_dao_is_member(group_id, username_to_add))
    {
        send_line(client_fd, "ERR User is already a member of this group\n");
        log_activity("[ERROR] GROUP_ADD_MEMBER: User %s already in group %d", username_to_add, group_id);
        return -1;
    }

    // Call DAO to do the work
    if (group_dao_add_member(group_id, username_to_add, "member") != 0)
    {
        send_line(client_fd, "ERR Failed to add member to database\n");
        log_activity("[ERROR] GROUP_ADD_MEMBER: Failed to add member to database");
        return -1;
    }

    char response[256];
    snprintf(response, sizeof(response), "OK Added %s to group %d\n", username_to_add, group_id);
    send_line(client_fd, response);
    log_activity("[INFO] GROUP_ADD_MEMBER: %s added %s to group %d", username, username_to_add, group_id);

    // Notify the added user if online (use tracker helper)
    char notify[256];
    snprintf(notify, sizeof(notify), "NOTIFY You have been added to group (ID: %d) by %s\n", group_id, username);
    notify_user(username_to_add, notify);

    return 0;
}

int group_service_remove_member(int client_fd, const char *username, int group_id, const char *username_to_remove)
{
    // Input validation
    if (!username || !username_to_remove || strlen(username) == 0 || strlen(username_to_remove) == 0 || group_id <= 0)
    {
        send_line(client_fd, "ERR Invalid parameters\n");
        log_activity("[ERROR] GROUP_REMOVE_MEMBER: Invalid input parameters");
        return -1;
    }

    // only owner can remove
    if (!group_dao_is_owner(group_id, username))
    {
        send_line(client_fd, "ERR Only group owner can remove members\n");
        log_activity("[ERROR] GROUP_REMOVE_MEMBER: User %s not owner of group %d", username, group_id);
        return -1;
    }

    // owner cannot remove themselves
    if (strcmp(username, username_to_remove) == 0)
    {
        send_line(client_fd, "ERR Owner cannot remove themselves (use LEAVE_GROUP or delete group)\n");
        log_activity("[ERROR] GROUP_REMOVE_MEMBER: Owner %s tried to remove self from group %d", username, group_id);
        return -1;
    }

    // target must be member
    if (!group_dao_is_member(group_id, username_to_remove))
    {
        send_line(client_fd, "ERR User is not a member of this group\n");
        log_activity("[ERROR] GROUP_REMOVE_MEMBER: User %s not in group %d", username_to_remove, group_id);
        return -1;
    }

    // Call DAO to do the work
    if (group_dao_remove_member(group_id, username_to_remove) != 0)
    {
        send_line(client_fd, "ERR Failed to remove member from database\n");
        log_activity("[ERROR] GROUP_REMOVE_MEMBER: Failed to remove member from database");
        return -1;
    }

    char resp[256];
    snprintf(resp, sizeof(resp), "OK Removed %s from group %d\n", username_to_remove, group_id);
    send_line(client_fd, resp);
    log_activity("[INFO] GROUP_REMOVE_MEMBER: %s removed %s from group %d", username, username_to_remove, group_id);

    // Notify the removed user if online
    char notify[256];
    snprintf(notify, sizeof(notify), "NOTIFY You have been removed from group %d by owner\n", group_id);
    notify_user(username_to_remove, notify);

    return 0;
}

int group_service_send_message(int client_fd, const char *username, int group_id, const char *message)
{
    // Input validation
    if (!username || !message || strlen(username) == 0 || strlen(message) == 0 || group_id <= 0)
    {
        send_line(client_fd, "ERR Invalid parameters\n");
        log_activity("[ERROR] GROUP_SEND_MESSAGE: Invalid input parameters");
        return -1;
    }

    // user must be member
    if (!group_dao_is_member(group_id, username))
    {
        send_line(client_fd, "ERR You must be a member of this group to send messages\n");
        log_activity("[ERROR] GROUP_SEND_MESSAGE: User %s not member of group %d", username, group_id);
        return -1;
    }

    // Call DAO to save message
    if (group_dao_save_message(group_id, username, message) != 0)
    {
        send_line(client_fd, "ERR Failed to save message\n");
        log_activity("[ERROR] GROUP_SEND_MESSAGE: Failed to save message");
        return -1;
    }

    log_activity("[INFO] GROUP_SEND_MESSAGE: %s sent to group %d", username, group_id);

    // Get members and broadcast (service handles notification logic)
    char **members;
    int member_count;

    if (group_dao_get_members(group_id, &members, &member_count) != 0 || member_count == 0)
    {
        send_line(client_fd, "OK Message saved but no members to broadcast to\n");
        return 0;
    }

    char broadcast_msg[4096];
    snprintf(broadcast_msg, sizeof(broadcast_msg), "GROUP_MSG [%d] %s: %s\n", group_id, username, message);

    int broadcast_count = 0;
    for (int i = 0; i < member_count; i++)
    {
        notify_user(members[i], broadcast_msg);
        broadcast_count++;
        free(members[i]);
    }
    free(members);

    char confirm[256];
    snprintf(confirm, sizeof(confirm), "OK Message sent to %d online member(s)\n", broadcast_count);
    send_line(client_fd, confirm);

    return 0;
}

int group_service_list_groups(int client_fd, const char *username) {
    // Input validation
    if (!username || strlen(username) == 0) {
        send_line(client_fd, "ERR Invalid username\n");
        log_activity("[ERROR] GROUP_LIST: Invalid username");
        return -1;
    }

    // Call DAO to get groups
    char **groups;
    int group_count;

    if (group_dao_list_user_groups(username, &groups, &group_count) != 0) {
        send_line(client_fd, "ERR Failed to retrieve group list\n");
        log_activity("[ERROR] GROUP_LIST: Failed to retrieve group list for %s", username);
        return -1;
    }

    if (group_count == 0) {
        send_line(client_fd, "OK You are not a member of any groups\n");
        return 0;
    }

    char header[256];
    snprintf(header, sizeof(header), "OK GROUP_LIST (%d groups):\n", group_count);
    send_line(client_fd, header);

    for (int i = 0; i < group_count; i++) {
        send_line(client_fd, groups[i]);
        send_line(client_fd, "\n");
        free(groups[i]);
    }
    free(groups);
    send_line(client_fd, "END_GROUP_LIST\n");

    log_activity("[INFO] GROUP_LIST: %s listed %d groups", username, group_count);
    return 0;
}

int group_service_get_history(int client_fd, const char *username, int group_id, int limit, const char ***messages, int *msg_count) {
    // Input validation
    if (!username || strlen(username) == 0 || group_id <= 0) {
        send_line(client_fd, "ERR Invalid parameters\n");
        log_activity("[ERROR] GROUP_GET_HISTORY: Invalid input parameters");
        return -1;
    }

    // user must be member
    if (!group_dao_is_member(group_id, username)) {
        send_line(client_fd, "ERR You must be a member of this group to view history\n");
        log_activity("[ERROR] GROUP_GET_HISTORY: User %s not member of group %d", username, group_id);
        return -1;
    }

    if (group_dao_get_history(group_id, 50, &messages, &msg_count) != 0) {
        send_line(client_fd, "ERR Failed to retrieve chat history\n");
        return -1;
    }

    if (msg_count == 0) {
        send_line(client_fd, "OK No messages in group history\n");
        return 0;
    }

    char header[256];
    snprintf(header, sizeof(header), "OK GROUP_HISTORY [%d] (%d messages):\n", group_id, msg_count);
    send_line(client_fd, header);

    for (int i = 0; i < msg_count; i++) {
        send_line(client_fd, messages[i]);
        send_line(client_fd, "\n");
        free(messages[i]);
    }
    free(messages);
    send_line(client_fd, "END_HISTORY\n");

    log_activity("[INFO] GROUP_GET_HISTORY: %s viewed history of group %d", username, group_id);
    return 0;
}

int group_service_get_members(int client_fd, const char *username, int group_id) {
    // Input validation
    if (!username || strlen(username) == 0 || group_id <= 0) {
        send_line(client_fd, "ERR Invalid parameters\n");
        log_activity("[ERROR] GROUP_GET_MEMBERS: Invalid input parameters");
        return -1;
    }

    // Call DAO to get members
    char **members;
    int member_count;

    if (group_dao_get_members(group_id, &members, &member_count) != 0) {
        send_line(client_fd, "ERR Failed to get group members\n");
        log_activity("[ERROR] GROUP_GET_MEMBERS: Failed to get members of group %d", group_id);
        return -1;
    }

    if (member_count == 0) {
        send_line(client_fd, "OK No members in this group\n");
        return 0;
    }

    char header[256];
    snprintf(header, sizeof(header), "OK GROUP_MEMBERS [%d] (%d members):\n", group_id, member_count);
    send_line(client_fd, header);

    for (int i = 0; i < member_count; i++) {
        send_line(client_fd, members[i]);
        send_line(client_fd, "\n");
        free(members[i]);
    }
    free(members);
    send_line(client_fd, "END_MEMBER_LIST\n");

    log_activity("[INFO] GROUP_GET_MEMBERS: %s viewed members of group %d", username, group_id);
    return 0;
}
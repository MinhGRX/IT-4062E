#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "controllers/group_controller.h"
#include "services/group_service.h"
#include "dao/group_dao.h"
#include "network.h"
#include "globals.h"
#include "services/log_service.h"

extern ActiveUser online_users[MAX_CLIENTS];
extern pthread_mutex_t online_mutex;
extern void log_activity(const char *fmt, ...);

void group_controller_create(int client_fd, char *username, const char *group_name)
{
    // Delegate to service layer for business logic and validation
    group_service_create(client_fd, username, group_name);
}

void group_controller_add_member(int client_fd, const char *username, const char *group_id_str, const char *username_to_add)
{
    // Parse and validate group ID at controller level
    if (!group_id_str || strlen(group_id_str) == 0)
    {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0)
    {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Delegate to service layer for business logic
    group_service_add_member(client_fd, username, group_id, username_to_add);
}

void group_controller_remove_member(int client_fd, const char *username, const char *group_id_str, const char *username_to_remove)
{
    // Parse and validate group ID at controller level
    if (!group_id_str || strlen(group_id_str) == 0)
    {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0)
    {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Delegate to service layer for business logic
    group_service_remove_member(client_fd, username, group_id, username_to_remove);
}

void group_controller_leave(int client_fd, const char *username, const char *group_id_str)
{
    // Validation
    if (!group_id_str || strlen(group_id_str) == 0)
    {
        send_line(client_fd, "ERR Invalid command format. Use: LEAVE_GROUP <group_id>\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0)
    {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Check if user is a member
    if (!group_dao_is_member(group_id, username))
    {
        send_line(client_fd, "ERR You are not a member of this group\n");
        return;
    }

    // Prevent owner from leaving
    if (group_dao_is_owner(group_id, username))
    {
        send_line(client_fd, "ERR Owner cannot leave group. Delete the group or transfer ownership first\n");
        log_activity("GROUP_LEAVE: Owner %s tried to leave group %d (blocked)", username, group_id);
        return;
    }

    // Remove the user
    if (group_dao_remove_member(group_id, username) == 0)
    {
        char response[256];
        snprintf(response, sizeof(response), "OK You have left group %d\n", group_id);
        send_line(client_fd, response);
        log_activity("GROUP_LEAVE: User %s left group %d", username, group_id);

        // Reset current group if needed
        int idx = get_user_index_by_fd(client_fd);
        if (idx != -1 && online_users[idx].current_group_id == group_id)
        {
            online_users[idx].current_group_id = -1;
            memset(online_users[idx].current_group_name, 0, sizeof(online_users[idx].current_group_name));
        }
        pthread_mutex_unlock(&online_mutex);
    }
    else
    {
        send_line(client_fd, "ERR Failed to leave group\n");
        log_activity("[ERROR] GROUP_LEAVE: Failed to remove %s from group %d", username, group_id);
    }
}

void group_controller_send_message(int client_fd, const char *username, const char *group_id_str, const char *message)
{
    // Parse and validate group ID
    if (!group_id_str || strlen(group_id_str) == 0)
    {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0)
    {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Delegate to service layer for business logic
    group_service_send_message(client_fd, username, group_id, message);
}

void group_controller_get_history(int client_fd, const char *username, const char *group_id_str)
{
    // Validation
    if (!group_id_str || strlen(group_id_str) == 0)
    {
        send_line(client_fd, "ERR Invalid command format. Use: GROUP_HISTORY <group_id>\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0)
    {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Check if username is a member
    if (!group_dao_is_member(group_id, username))
    {
        send_line(client_fd, "ERR You must be a member of this group to view history\n");
        return;
    }

    // Get history
    char **messages;
    int msg_count;

    if (group_dao_get_history(group_id, 50, &messages, &msg_count) != 0)
    {
        send_line(client_fd, "ERR Failed to retrieve chat history\n");
        return;
    }

    if (msg_count == 0)
    {
        send_line(client_fd, "OK No messages in this group yet\n");
        return;
    }

    char header[256];
    snprintf(header, sizeof(header), "OK GROUP_HISTORY [%d] (%d messages)\n", group_id, msg_count);
    send_line(client_fd, header);

    for (int i = msg_count - 1; i >= 0; i--)
    {
        send_line(client_fd, messages[i]);
        send_line(client_fd, "\n");
        free(messages[i]);
    }

    free(messages);
    send_line(client_fd, "END_HISTORY\n");

    log_activity("GROUP_HISTORY: %s retrieved %d messages from group %d", username, msg_count, group_id);
}

void group_controller_get_members(int client_fd, const char *username, const char *group_id_str)
{
    // Validation
    if (!group_id_str || strlen(group_id_str) == 0)
    {
        send_line(client_fd, "ERR Invalid command format. Use: GROUP_MEMBERS <group_id>\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0)
    {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    char **members;
    int count;

    if (group_dao_get_members(group_id, &members, &count) != 0)
    {
        send_line(client_fd, "ERR Failed to get group members\n");
        return;
    }

    if (count == 0)
    {
        send_line(client_fd, "OK No members in this group\n");
        return;
    }

    char header[256];
    snprintf(header, sizeof(header), "OK GROUP_MEMBERS [%d] (%d members)\n", group_id, count);
    send_line(client_fd, header);

    for (int i = 0; i < count; i++)
    {
        char line[512];

        if (group_dao_is_owner(group_id, members[i]))
        {
            snprintf(line, sizeof(line), "- %s (owner)\n", members[i]);
        }
        else
        {
            snprintf(line, sizeof(line), "- %s\n", members[i]);
        }

        send_line(client_fd, line);
        free(members[i]);
    }

    free(members);
    send_line(client_fd, "END_MEMBERS\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "controllers/group_controller.h"
#include "dao/group_dao.h"
#include "dao/friend_dao.h"
#include "network.h"
#include "globals.h"
#include "services/log_service.h"

extern ActiveUser online_users[MAX_CLIENTS];
extern pthread_mutex_t online_mutex;
extern void log_activity(const char *fmt, ...);

void group_controller_create(int client_fd, char *username, const char *group_name) {
    // Validation
    if (!group_name || strlen(group_name) == 0) {
        send_line(client_fd, "ERR Invalid group name\n");
        return;
    }

    int group_id = 0;
    if (group_dao_create(group_name, username, &group_id) == 0) {
        char ok[128];
        snprintf(ok, sizeof(ok), "OK Group created successfully (ID:%d)\n", group_id);
        send_line(client_fd, ok);
    } else {
        send_line(client_fd, "ERR Group creation failed (name may exist)\n");
    }
}

void group_controller_add_member(int client_fd, const char *username, const char *group_id_str, const char *username_to_add) {
    // Validation
    if (!group_id_str || !username_to_add || strlen(group_id_str) == 0 || strlen(username_to_add) == 0) {
        send_line(client_fd, "ERR Invalid command format. Use: GROUP_ADD <group_id> <username>\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0) {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Check if username is a member
    if (!group_dao_is_member(group_id, username)) {
        send_line(client_fd, "ERR You must be a member of this group to add others\n");
        log_activity("GROUP_ADD: User %s tried to add %s but is not a member of group %d", 
               username, username_to_add, group_id);
        return;
    }

    // Check if they are friends
    printf("[DEBUG] Checking if %s and %s are friends...\n", username, username_to_add);
    if (!friend_dao_are_friends(username, username_to_add)) {
        send_line(client_fd, "ERR You can only add friends to the group\n");
        log_activity("GROUP_ADD: User %s tried to add %s to group %d but they are not friends", 
               username, username_to_add, group_id);
        printf("[DEBUG] Friendship check FAILED\n");
        return;
    }
    printf("[DEBUG] Friendship check PASSED\n");

    // Check if already a member
    if (group_dao_is_member(group_id, username_to_add)) {
        send_line(client_fd, "ERR User is already a member of this group\n");
        return;
    }

    // Add member
    if (group_dao_add_member(group_id, username_to_add, "member") == 0) {
        char response[256];
        snprintf(response, sizeof(response), "OK Added %s to group %d\n", username_to_add, group_id);
        send_line(client_fd, response);
        log_activity("GROUP_ADD: User %s added %s to group %d", username, username_to_add, group_id);

        // Notify the added user if online
        pthread_mutex_lock(&online_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (online_users[i].fd != -1 && 
                strcmp(online_users[i].username, username_to_add) == 0) {
                char notify[256];
                snprintf(notify, sizeof(notify), 
                    "NOTIFY You have been added to group (ID: %d) by %s\n", 
                    group_id, username);
                send_line(online_users[i].fd, notify);
                log_activity("GROUP_ADD: Notified %s about being added to group %d", username_to_add, group_id);
                break;
            }
        }
        pthread_mutex_unlock(&online_mutex);
    } else {
        send_line(client_fd, "ERR Failed to add member to group\n");
        log_activity("GROUP_ADD: Failed to add %s to group %d", username_to_add, group_id);
    }
}

void group_controller_remove_member(int client_fd, const char *username, const char *group_id_str, const char *username_to_remove) {
    // Validation
    if (!group_id_str || !username_to_remove || strlen(group_id_str) == 0 || strlen(username_to_remove) == 0) {
        send_line(client_fd, "ERR Invalid command format. Use: GROUP_REMOVE <group_id> <username>\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0) {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Check if username is owner
    if (!group_dao_is_owner(group_id, username)) {
        send_line(client_fd, "ERR Only group owner can remove members\n");
        return;
    }

    // Prevent removing owner
    if (strcmp(username, username_to_remove) == 0) {
        send_line(client_fd, "ERR Owner cannot remove themselves (use LEAVE_GROUP or delete group)\n");
        return;
    }

    // Ensure target is a member
    if (!group_dao_is_member(group_id, username_to_remove)) {
        send_line(client_fd, "ERR User is not a member of this group\n");
        return;
    }

    // Remove member
    if (group_dao_remove_member(group_id, username_to_remove) == 0) {
        char resp[256];
        snprintf(resp, sizeof(resp), "OK Removed %s from group %d\n", username_to_remove, group_id);
        send_line(client_fd, resp);
        log_activity("GROUP_REMOVE: owner %s removed %s from group %d", username, username_to_remove, group_id);

        // Notify removed user if online
        pthread_mutex_lock(&online_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (online_users[i].fd != -1 && strcmp(online_users[i].username, username_to_remove) == 0) {
                char notify[256];
                snprintf(notify, sizeof(notify), "NOTIFY You have been removed from group %d by owner\n", group_id);
                send_line(online_users[i].fd, notify);
                break;
            }
        }
        pthread_mutex_unlock(&online_mutex);
    } else {
        send_line(client_fd, "ERR Failed to remove member\n");
    }
}

void group_controller_leave(int client_fd, const char *username, const char *group_id_str) {
    // Validation
    if (!group_id_str || strlen(group_id_str) == 0) {
        send_line(client_fd, "ERR Invalid command format. Use: LEAVE_GROUP <group_id>\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0) {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Check if user is a member
    if (!group_dao_is_member(group_id, username)) {
        send_line(client_fd, "ERR You are not a member of this group\n");
        return;
    }

    // Prevent owner from leaving
    if (group_dao_is_owner(group_id, username)) {
        send_line(client_fd, "ERR Owner cannot leave group. Delete the group or transfer ownership first\n");
        log_activity("GROUP_LEAVE: Owner %s tried to leave group %d (blocked)", username, group_id);
        return;
    }

    // Remove the user
    if (group_dao_remove_member(group_id, username) == 0) {
        char response[256];
        snprintf(response, sizeof(response), "OK You have left group %d\n", group_id);
        send_line(client_fd, response);
        log_activity("GROUP_LEAVE: User %s left group %d", username, group_id);

        // Reset current group if needed
        pthread_mutex_lock(&online_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (online_users[i].fd == client_fd) {
                if (online_users[i].current_group_id == group_id) {
                    online_users[i].current_group_id = -1;
                    memset(online_users[i].current_group_name, 0, sizeof(online_users[i].current_group_name));
                }
                break;
            }
        }
        pthread_mutex_unlock(&online_mutex);
    } else {
        send_line(client_fd, "ERR Failed to leave group\n");
        log_activity("GROUP_LEAVE: Failed to remove %s from group %d", username, group_id);
    }
}

void group_controller_send_message(int client_fd, const char *username, const char *group_id_str, const char *message) {
    // Validation
    if (!group_id_str || !message || strlen(group_id_str) == 0 || strlen(message) == 0) {
        send_line(client_fd, "ERR Invalid command format. Use: GROUP_MSG <group_id> <message>\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0) {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Check if username is a member
    if (!group_dao_is_member(group_id, username)) {
        send_line(client_fd, "ERR You must be a member of this group to send messages\n");
        return;
    }

    // Save message
    if (group_dao_save_message(group_id, username, message) != 0) {
        send_line(client_fd, "ERR Failed to save message\n");
        log_activity("GROUP_MSG: Failed to save message from %s to group %d", username, group_id);
        return;
    }

    log_activity("GROUP_MSG: %s sent message to group %d: %s", username, group_id, message);

    // Get members and broadcast
    char **members;
    int member_count;
    
    if (group_dao_get_members(group_id, &members, &member_count) != 0 || member_count == 0) {
        send_line(client_fd, "OK Message saved but no members to broadcast to\n");
        return;
    }

    char broadcast_msg[4096];
    snprintf(broadcast_msg, sizeof(broadcast_msg), "GROUP_MSG [%d] %s: %s\n", group_id, username, message);

    pthread_mutex_lock(&online_mutex);
    int broadcast_count = 0;
    
    for (int i = 0; i < member_count; i++) {
        for (int j = 0; j < MAX_CLIENTS; j++) {
            if (online_users[j].fd != -1 && 
                strcmp(online_users[j].username, members[i]) == 0) {
                send_line(online_users[j].fd, broadcast_msg);
                broadcast_count++;
                break;
            }
        }
        free(members[i]);
    }
    pthread_mutex_unlock(&online_mutex);

    free(members);

    char confirm[256];
    snprintf(confirm, sizeof(confirm), "OK Message sent to %d online member(s)\n", broadcast_count);
    send_line(client_fd, confirm);
}

void group_controller_get_history(int client_fd, const char *username, const char *group_id_str) {
    // Validation
    if (!group_id_str || strlen(group_id_str) == 0) {
        send_line(client_fd, "ERR Invalid command format. Use: GROUP_HISTORY <group_id>\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0) {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Check if username is a member
    if (!group_dao_is_member(group_id, username)) {
        send_line(client_fd, "ERR You must be a member of this group to view history\n");
        return;
    }

    // Get history
    char **messages;
    int msg_count;

    if (group_dao_get_history(group_id, 50, &messages, &msg_count) != 0) {
        send_line(client_fd, "ERR Failed to retrieve chat history\n");
        return;
    }

    if (msg_count == 0) {
        send_line(client_fd, "OK No messages in this group yet\n");
        return;
    }

    char header[256];
    snprintf(header, sizeof(header), "OK GROUP_HISTORY [%d] (%d messages)\n", group_id, msg_count);
    send_line(client_fd, header);

    for (int i = msg_count - 1; i >= 0; i--) {
        send_line(client_fd, messages[i]);
        send_line(client_fd, "\n");
        free(messages[i]);
    }

    free(messages);
    send_line(client_fd, "END_HISTORY\n");

    log_activity("GROUP_HISTORY: %s retrieved %d messages from group %d", username, msg_count, group_id);
}

void group_controller_get_members(int client_fd, const char *username, const char *group_id_str) {
    // Validation
    if (!group_id_str || strlen(group_id_str) == 0) {
        send_line(client_fd, "ERR Invalid command format. Use: GROUP_MEMBERS <group_id>\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0) {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    char **members;
    int count;

    if (group_dao_get_members(group_id, &members, &count) != 0) {
        send_line(client_fd, "ERR Failed to get group members\n");
        return;
    }

    if (count == 0) {
        send_line(client_fd, "OK No members in this group\n");
        return;
    }

    char header[256];
    snprintf(header, sizeof(header), "OK GROUP_MEMBERS [%d] (%d members)\n", group_id, count);
    send_line(client_fd, header);

    for (int i = 0; i < count; i++) {
        char line[512];
        
        if (group_dao_is_owner(group_id, members[i])) {
            snprintf(line, sizeof(line), "- %s (owner)\n", members[i]);
        } else {
            snprintf(line, sizeof(line), "- %s\n", members[i]);
        }
        
        send_line(client_fd, line);
        free(members[i]);
    }

    free(members);
    send_line(client_fd, "END_MEMBERS\n");
}
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "../../include/controllers/group_controller.h"
#include "../../include/dao/group_dao.h"
#include "../../include/network.h"
#include "../../include/globals.h"

extern ActiveUser online_users[MAX_CLIENTS];
extern pthread_mutex_t online_mutex;

void group_controller_create(int client_fd, char *username, const char *group_name) {
    int gid = 0;
    if (group_dao_create(group_name, username, &gid) == 0) {
        char ok[128];
        snprintf(ok, sizeof(ok), "OK Group created successfully (ID:%d)\n", gid);
        send_line(client_fd, ok);
    } else {
        send_line(client_fd, "ERR Group creation failed (name may exist)\n");
    }
}

void group_controller_add_member(int client_fd, int group_id, const char *requester, const char *username_to_add) {
    // Validation
    if (!requester || strlen(requester) == 0) {
        send_line(client_fd, "ERR Invalid requester\n");
        return;
    }
    
    if (!username_to_add || strlen(username_to_add) == 0) {
        send_line(client_fd, "ERR Invalid username to add\n");
        return;
    }
    
    // Check if requester is a member of the group (ANY member can add)
    if (!group_dao_is_member(group_id, requester)) {
        send_line(client_fd, "ERR You must be a member of this group to add others\n");
        printf("[LOG] User %s tried to add %s but is not a member of group %d\n", 
               requester, username_to_add, group_id);
        return;
    }
    
    // Check if requester and username_to_add are friends
    if (!friend_dao_are_friends(requester, username_to_add)) {
        send_line(client_fd, "ERR You can only add friends to the group\n");
        log_activity("GROUP_ADD: User %s tried to add %s to group %d but they are not friends", 
               requester, username_to_add, group_id);
        return;
    }

    // Check if user already is a member
    if (group_dao_is_member(group_id, username_to_add)) {
        send_line(client_fd, "ERR User is already a member of this group\n");
        return;
    }
    
    // Add member to group
    int result = group_dao_add_member(group_id, username_to_add, "member");
    if (result == 0) {
        char response[256];
        snprintf(response, sizeof(response), "OK Added %s to group %d\n", username_to_add, group_id);
        send_line(client_fd, response);
        printf("[LOG] User %s added %s to group %d\n", requester, username_to_add, group_id);
        
        // Notify the added user if they're online
        pthread_mutex_lock(&online_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (online_users[i].fd != -1 && 
                strcmp(online_users[i].username, username_to_add) == 0) {
                char notify[256];
                snprintf(notify, sizeof(notify), 
                    "NOTIFY You have been added to group (ID: %d) by %s\n", 
                    group_id, requester);
                send_line(online_users[i].fd, notify);
                printf("[LOG] Notified %s about being added to group %d\n", username_to_add, group_id);
                break;
            }
        }
        pthread_mutex_unlock(&online_mutex);
    } else {
        send_line(client_fd, "ERR Failed to add member to group\n");
        printf("[LOG] Failed to add %s to group %d\n", username_to_add, group_id);
    }
}

void group_controller_remove_member(int client_fd, int group_id, const char *owner, const char *username_to_remove) {
    // Validate input
    if (!owner || !username_to_remove || strlen(owner) == 0 || strlen(username_to_remove) == 0) {
        send_line(client_fd, "ERR Invalid parameters\n");
        return;
    }

    // Check permission: only owner
    if (!group_dao_is_owner(group_id, owner)) {
        send_line(client_fd, "ERR Only group owner can remove members\n");
        return;
    }

    // Prevent removing owner
    if (strcmp(owner, username_to_remove) == 0) {
        send_line(client_fd, "ERR Owner cannot remove themselves (use LEAVE_GROUP or delete group)\n");
        return;
    }

    // Ensure target is in group
    if (!group_dao_is_member(group_id, username_to_remove)) {
        send_line(client_fd, "ERR User is not a member of this group\n");
        return;
    }

    // Remove
    if (group_dao_remove_member(group_id, username_to_remove) == 0) {
        char resp[256];
        snprintf(resp, sizeof(resp), "OK Removed %s from group %d\n", username_to_remove, group_id);
        send_line(client_fd, resp);
        log_activity("GROUP_REMOVE: owner %s removed %s from group %d", owner, username_to_remove, group_id);

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
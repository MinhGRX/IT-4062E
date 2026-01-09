#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "controllers/group_controller.h"
#include "dao/group_dao.h"
#include "database.h"
#include "network.h"
#include "globals.h"
#include "services/log_service.h"

extern ActiveUser online_users[MAX_CLIENTS];
extern pthread_mutex_t online_mutex;
extern void log_activity(const char *fmt, ...);

void group_controller_create(int client_fd, char *username, const char *group_name) {
    // Delegate to service layer for business logic and validation
    group_service_create(client_fd, username, group_name);
}

void group_controller_add_member(int client_fd, const char *username, const char *group_id_str, const char *username_to_add) {
    // Parse and validate group ID at controller level
    if (!group_id_str || strlen(group_id_str) == 0) {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0) {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Delegate to service layer for business logic
    group_service_add_member(client_fd, username, group_id, username_to_add);
}

void group_controller_remove_member(int client_fd, const char *username, const char *group_id_str, const char *username_to_remove) {
    // Parse and validate group ID at controller level
    if (!group_id_str || strlen(group_id_str) == 0) {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0) {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Delegate to service layer for business logic
    group_service_remove_member(client_fd, username, group_id, username_to_remove);
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

    // Remove member
    if (group_service_remove_member(client_fd, username, group_id, username) != 0) {
        send_line(client_fd, "ERR Failed to leave group\n");
        log_activity("GROUP_LEAVE: %s failed to leave group %d", username, group_id);
        return; 
    }
}

void group_controller_send_message(int client_fd, const char *username, const char *group_id_str, const char *message) {
    // Parse and validate group ID
    if (!group_id_str || strlen(group_id_str) == 0) {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    int group_id = atoi(group_id_str);
    if (group_id <= 0) {
        send_line(client_fd, "ERR Invalid group ID\n");
        return;
    }

    // Delegate to service layer for business logic
    group_service_send_message(client_fd, username, group_id, message);
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

    // Get history
    char **messages;
    int msg_count;

    if (group_service_get_history(client_fd, username, group_id, 50, &messages, &msg_count) != 0) {
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

    // Get members
    if (group_service_get_members(client_fd, username, group_id) != 0) {
        send_line(client_fd, "ERR Failed to retrieve group members\n");
        return;
    }
    
    log_activity("GROUP_MEMBERS: %s viewed members of group %d", username, group_id);
}

void group_controller_list(int client_fd, const char *username) {
    // List groups
    if (group_service_list_groups(client_fd, username) != 0) {
        send_line(client_fd, "ERR Failed to list groups\n");
        return;
    }

    log_activity("LIST_GROUPS: %s listed their groups", username);
}

// Enter group chat - load history and set current group
void group_controller_enter(int client_idx, int group_id)
{
    char *username = online_users[client_idx].username;
    int fd = online_users[client_idx].fd;

    // Verify user is member of group
    if (group_dao_is_member(username, group_id) != 0) {
        send_line(fd, "ERR: You are not a member of this group.\n");
        return;
    }

    // Lock and set current group
    pthread_mutex_lock(&online_mutex);
    online_users[client_idx].current_group_id = group_id;
    strncpy(online_users[client_idx].current_group_name, 
            group_dao_get_group_name(group_id), 63);
    online_users[client_idx].current_group_name[63] = '\0';
    online_users[client_idx].in_group_chat = 1;
    pthread_mutex_unlock(&online_mutex);

    // Load and send chat history (last 50 messages)
    group_dao_get_history(fd, username, group_id, 50);

    // Send confirmation
    char msg[256];
    snprintf(msg, sizeof(msg), "OK: Đã vào nhóm %d (%s).\n", 
             group_id, online_users[client_idx].current_group_name);
    send_line(fd, msg);

    // Notify other members that user joined
    char notify[512];
    snprintf(notify, sizeof(notify), "NOTIFY: %s vừa vào nhóm.\n", username);
    broadcast_to_group_except(group_id, fd, notify);

    log_info("User %s entered group %d", username, group_id);
}

// Leave group chat - return to main hall
void group_controller_leave(int client_idx)
{
    char *username = online_users[client_idx].username;
    int fd = online_users[client_idx].fd;

    // Get group info before clearing
    pthread_mutex_lock(&online_mutex);
    int group_id = online_users[client_idx].current_group_id;
    char group_name[64];
    strncpy(group_name, online_users[client_idx].current_group_name, 63);
    
    // Clear group chat state
    online_users[client_idx].current_group_id = -1;
    memset(online_users[client_idx].current_group_name, 0, 64);
    online_users[client_idx].in_group_chat = 0;
    pthread_mutex_unlock(&online_mutex);

    // Send confirmation
    send_line(fd, "OK: Đã quay lại sảnh chính.\n");

    // Notify other members that user left
    char notify[512];
    snprintf(notify, sizeof(notify), "NOTIFY: %s đã rời khỏi nhóm.\n", username);
    broadcast_to_group_except(group_id, fd, notify);

    log_info("User %s left group %d", username, group_id);
}

// Send message to group with real-time broadcast
void group_controller_send(int client_idx, const char *content)
{
    char *username = online_users[client_idx].username;
    int fd = online_users[client_idx].fd;

    // Get group info
    pthread_mutex_lock(&online_mutex);
    int group_id = online_users[client_idx].current_group_id;
    char group_name[64];
    strncpy(group_name, online_users[client_idx].current_group_name, 63);
    pthread_mutex_unlock(&online_mutex);

    if (group_id <= 0) {
        send_line(fd, "ERR: Bạn chưa vào nhóm chat nào.\n");
        return;
    }

    if (!content || strlen(content) == 0) {
        send_line(fd, "ERR: Tin nhắn không được để trống.\n");
        return;
    }

    // Save message to database
    if (group_dao_save_message(group_id, username, content) != 0) {
        send_line(fd, "ERR: Lỗi khi lưu tin nhắn.\n");
        return;
    }

    // Send confirmation to sender
    send_line(fd, "OK: Tin nhắn đã được gửi.\n");

    // Broadcast to all online members in group
    char broadcast_msg[4096];
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);

    snprintf(broadcast_msg, sizeof(broadcast_msg), 
             "MSG_GROUP [%d] %s [%s]: %s\n", 
             group_id, username, timestamp, content);

    broadcast_to_group(group_id, broadcast_msg);

    log_info("Message from %s in group %d: %s", username, group_id, content);
}

// Handle group chat line commands (similar to one-to-one chat)
void group_controller_handle_line(int client_idx, char *line)
{
    int fd = online_users[client_idx].fd;
    char *username = online_users[client_idx].username;

    pthread_mutex_lock(&online_mutex);
    int group_id = online_users[client_idx].current_group_id;
    pthread_mutex_unlock(&online_mutex);

    if (group_id <= 0) {
        send_line(fd, "ERR: Bạn chưa vào nhóm chat nào.\n");
        return;
    }

    // Handle commands while in group
    if (strcmp(line, "LEAVE") == 0) {
        group_controller_leave(client_idx);
    }
    else if (strcmp(line, "HISTORY") == 0) {
        log_activity("User %s requested history for group %d", username, group_id);
        group_dao_get_history(fd, username, group_id, 50);
    }
    else if (strcmp(line, "MEMBERS") == 0) {
        group_controller_list_members(client_idx, group_id);
    }
    else if (strncmp(line, "SEND ", 5) == 0) {
        group_controller_send(client_idx, line + 5);
    }
    else {
        // Default: treat any input as message
        group_controller_send(client_idx, line);
    }
}

// List all members in current group
void group_controller_list_members(int client_idx, int group_id)
{
    int fd = online_users[client_idx].fd;
    char *username = online_users[client_idx].username;

    send_line(fd, "--- Danh sách thành viên nhóm ---\n");
    
    // Get members from database
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT gm.username, gm.role, u.is_online "
             "FROM group_member gm "
             "LEFT JOIN users u ON gm.username = u.username "
             "WHERE gm.group_id = %d "
             "ORDER BY gm.role DESC, gm.username ASC",
             group_id);

    PGconn *conn = get_db_connection();
    PGresult *res = PQexec(conn, query);

    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
        log_error("Failed to get group members: %s", PQerrorMessage(conn));
        send_line(fd, "ERR: Lỗi khi lấy danh sách thành viên.\n");
        if (res) PQclear(res);
        return;
    }

    int ntuples = PQntuples(res);
    if (ntuples == 0) {
        send_line(fd, "Nhóm không có thành viên nào.\n");
        PQclear(res);
        return;
    }

    char member_list[4096] = {0};
    for (int i = 0; i < ntuples; i++) {
        const char *member_name = PQgetvalue(res, i, 0);
        const char *role = PQgetvalue(res, i, 1);
        const char *is_online = PQgetvalue(res, i, 2);

        char status = strcmp(is_online, "1") == 0 ? '*' : ' ';
        char role_display[16];
        
        if (strcmp(role, "owner") == 0) {
            snprintf(role_display, sizeof(role_display), "[Chủ]");
        } else {
            snprintf(role_display, sizeof(role_display), "");
        }

        char line_buf[256];
        snprintf(line_buf, sizeof(line_buf), "%c %s %s\n", 
                 status, member_name, role_display);
        strcat(member_list, line_buf);
    }

    PQclear(res);

    send_line(fd, member_list);
    send_line(fd, "--- Kết thúc danh sách ---\n");
}
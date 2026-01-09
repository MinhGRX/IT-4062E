#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "controllers/chat_controller.h"
#include "dao/chat_dao.h"
#include "services/log_service.h"
#include "database.h"
#include "network.h"
#include "globals.h"

extern void log_activity(const char *fmt, ...);
// 1. Vào box chat
void chat_controller_enter(int client_idx, const char *target)
{
    pthread_mutex_lock(&online_mutex);
    strncpy(online_users[client_idx].chatting_with, target, 63);
    online_users[client_idx].chatting_with[63] = '\0';
    pthread_mutex_unlock(&online_mutex);
    chat_dao_get_history(online_users[client_idx].fd, online_users[client_idx].username, target);
    chat_dao_mark_delivered(online_users[client_idx].username, target);
}

// 2. Thoát box chat
void chat_controller_leave(int client_idx)
{
    pthread_mutex_lock(&online_mutex);
    memset(online_users[client_idx].chatting_with, 0, 64);
    pthread_mutex_unlock(&online_mutex);

    send_line(online_users[client_idx].fd, "OK: Back to main lobby.\n");
}

void chat_controller_send(int client_idx, const char *content)
{
    char *me = online_users[client_idx].username;
    char *target = online_users[client_idx].chatting_with;
    int my_fd = online_users[client_idx].fd;

    int target_fd = -1;
    int is_target_looking_at_me = 0;
    pthread_mutex_lock(&online_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (online_users[i].fd != -1 && strcmp(online_users[i].username, target) == 0)
        {
            target_fd = online_users[i].fd;
            if (strcmp(online_users[i].chatting_with, me) == 0)
            {
                is_target_looking_at_me = 1;
            }
            break;
        }
    }
    pthread_mutex_unlock(&online_mutex);

    if (is_target_looking_at_me)
    {
        char buf[BUF_SIZE];
        snprintf(buf, sizeof(buf), "MSG [%s]: %s\n", me, content);
        send_line(target_fd, buf);
        chat_dao_save_message(me, target, content, 1);
    }
    else
    {
        if (target_fd != -1)
        {
            char notify[BUF_SIZE];
            snprintf(notify, sizeof(notify), "NOTIFICATION: %s just send you message.\n", me);
            send_line(target_fd, notify);
        }
        chat_dao_save_message(me, target, content, 0);
    }
}

void chat_controller_handle_line(int client_idx, char *line)
{
    int my_fd = online_users[client_idx].fd;
    char *me = online_users[client_idx].username;
    char *target = online_users[client_idx].chatting_with;

    if (strcmp(line, "LEAVE") == 0)
    {
        chat_controller_leave(client_idx);
        log_activity("[INFO] User %s left chat with %s\n", me, target);
    }
    else if (strcmp(line, "HISTORY") == 0)
    {
        printf("[LOG] User %s requested history with %s\n", me, target);
        log_activity("[INFO] User %s requested chat history with %s\n", me, target);
        chat_dao_get_history(my_fd, me, target);
    }
    else if (strncmp(line, "SEND ", 5) == 0)
    {
        chat_controller_send(client_idx, line + 5);
        log_activity("[INFO] User %s sent message to %s: %s\n", me, target, line + 5);
    }
    else
    {
        // Mặc định gõ gì cũng là gửi tin
        chat_controller_send(client_idx, line);
    }
}
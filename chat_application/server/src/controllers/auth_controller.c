#include <stdio.h>    
#include <string.h>
#include <strings.h>
#include <stdlib.h> 
#include <libpq-fe.h>  
#include <pthread.h>
#include "controllers/auth_controller.h"
#include "controllers/chat_controller.h"
#include "controllers/group_controller.h"
#include "dao/friend_dao.h" 
#include "dao/chat_dao.h"
#include "database.h" 
#include "network.h"
#include "services/user_service.h"
#include "globals.h"

extern void log_activity(const char *fmt, ...);

void auth_controller_handle(int fd, char *cmd, char *user, char *pass, char *current_user, int *is_logged_in) {
    
    // 1. PING & REGISTER
    if (strcasecmp(cmd, "PING") == 0) {
        send_line(fd, "PONG\n");
        return;
    }

    if (strcmp(cmd, "REGISTER") == 0) {
        int res_reg = user_service_register(user, pass);
        if (res_reg == 1) {
            send_line(fd, "OK User registered\n");
            log_activity("REGISTER: New user %s", user);
        }
        else if (res_reg == -1) send_line(fd, "ERR User already exists\n");
        else send_line(fd, "ERR Registration failed\n");
        return;
    } 
    
    // 2. Xử lý LOGIN
    if (strcmp(cmd, "LOGIN") == 0) {
        if (user_service_login(user, pass)) {
            strncpy(current_user, user, 63);
            *is_logged_in = 1; 
            
            int my_idx = -1;
            pthread_mutex_lock(&online_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (online_users[i].fd == -1 || online_users[i].fd == fd) {
                    online_users[i].fd = fd;
                    strncpy(online_users[i].username, user, 63);
                    strcpy(online_users[i].chatting_with, "");
                    my_idx = i;
                    break;
                }
            }
            pthread_mutex_unlock(&online_mutex);

            // Cập nhật trạng thái DB
            char update_query[256];
            sprintf(update_query, "UPDATE \"User\" SET status = '1' WHERE username = '%s';", user);
            PQexec(conn, update_query);

            send_line(fd, "OK LOGIN successful\n");
            chat_dao_get_pending_senders(fd, user);
            
            friend_dao_get_list(fd, user);
            log_activity("LOGIN: %s is now online", user);
        } else {
            send_line(fd, "ERR Wrong username or password\n");
        }
    }

    // 3. Các lệnh yêu cầu đã Login
    else if (*is_logged_in) {
        if (strcmp(cmd, "LOGOUT") == 0) {
            char update_query[256];
            sprintf(update_query, "UPDATE \"User\" SET status = '0' WHERE username = '%s';", current_user);
            PQexec(conn, update_query);
            pthread_mutex_lock(&online_mutex);
            for(int i=0; i<MAX_CLIENTS; i++) {
                if(online_users[i].fd == fd) {
                    online_users[i].fd = -1;
                    memset(online_users[i].username, 0, 64);
                    break;
                }
            }
            pthread_mutex_unlock(&online_mutex);

            log_activity("LOGOUT: User %s has logged out", current_user);
            *is_logged_in = 0;
            memset(current_user, 0, 64);
            send_line(fd, "OK LOGOUT successful\n");
        }

        // Friend related commands
        else if (strcmp(cmd, "LIST_FRIEND") == 0) friend_dao_get_list(fd, current_user);
        else if (strcmp(cmd, "FRIEND_REQUEST") == 0) friend_dao_get_requests(fd, current_user);
        else if (strcmp(cmd, "ADD_FRIEND") == 0) friend_dao_add_request(fd, current_user, user);
        else if (strcmp(cmd, "ACCEPT_FRIEND") == 0) friend_dao_accept_request(fd, current_user, user);
        else if (strcmp(cmd, "DECLINE_FRIEND") == 0) friend_dao_decline_request(fd, current_user, user);
        else if (strcmp(cmd, "REMOVE_FRIEND") == 0) friend_dao_remove_friend(fd, current_user, user);
        
        // Group related commands
        else if (strcmp(cmd, "CREATE_GROUP") == 0) group_controller_create(fd, current_user, user);
        else if (strcmp(cmd, "GROUP_ADD") == 0) {
            if (user && pass && strlen(user) > 0 && strlen(pass) > 0) {
                int group_id = atoi(user);  // Convert arg1 to int
                printf("[LOG] Processing GROUP_ADD command: group_id=%d, username=%s from user %s\n", 
                    group_id, pass, current_user);
                group_controller_add_member(fd, group_id, current_user, pass);
            } else {
                send_line(fd, "ERR Invalid command format. Use: GROUP_ADD <group_id> <username>\n");
            }
        }
        else if (strcmp(cmd, "GROUP_REMOVE") == 0) {
            if (user && pass && strlen(user) > 0 && strlen(pass) > 0) {
                int group_id = atoi(user);
                group_controller_remove_member(fd, group_id, current_user, pass);
            } else {
                send_line(fd, "ERR Invalid command format. Use: GROUP_REMOVE <group_id> <username>\n");
            }
        }
        else if (strcmp(cmd, "LEAVE_GROUP") == 0) {
            // user = group_id (string)
            if (user && strlen(user) > 0) {
                int group_id = atoi(user);
                group_controller_leave(fd, group_id, current_user);
            } else {
                send_line(fd, "ERR Invalid command format. Use: LEAVE_GROUP <group_id>\n");
            }
        }


        // Chat related commands
        else if (strcmp(cmd, "CHAT") == 0) {
            int my_idx = -1;
            pthread_mutex_lock(&online_mutex);
            for(int i=0; i<MAX_CLIENTS; i++) if(online_users[i].fd == fd) { my_idx = i; break; }
            pthread_mutex_unlock(&online_mutex);

            if (my_idx != -1) {
                chat_controller_enter(my_idx, user);
            }
        }
    } else {
        send_line(fd, "ERR Please login first\n");
    }
}
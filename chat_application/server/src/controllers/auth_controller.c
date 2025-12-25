#include <stdio.h>    
#include <string.h>
#include <strings.h>
#include <libpq-fe.h>  
#include <pthread.h>
#include "controllers/auth_controller.h"
#include "dao/friend_dao.h" 
#include "database.h" 
#include "network.h"
#include "services/user_service.h"
#include "globals.h"

// Khai báo hàm log từ server.c
extern void log_activity(const char *fmt, ...);

void auth_controller_handle(int fd, char *cmd, char *user, char *pass, char *current_user, int *is_logged_in) {
    
    // 1. Xử lý PING
    if (strcasecmp(cmd, "PING") == 0) {
        send_line(fd, "PONG\n");
        return;
    }

    // 2. Xử lý REGISTER
    if (strcmp(cmd, "REGISTER") == 0) {
        int res_reg = user_service_register(user, pass);
        if (res_reg == 1) {
            send_line(fd, "OK User registered\n");
            log_activity("REGISTER: New user %s", user);
        }
        else if (res_reg == -1) send_line(fd, "ERR User already exists\n");
        else send_line(fd, "ERR Registration failed\n");
    } 
    
    // 3. Xử lý LOGIN
    else if (strcmp(cmd, "LOGIN") == 0) {
        if (user_service_login(user, pass)) {
            strncpy(current_user, user, 63);
            *is_logged_in = 1; 
            
            pthread_mutex_lock(&online_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (online_users[i].fd == -1 || online_users[i].fd == fd) {
                    online_users[i].fd = fd;
                    strncpy(online_users[i].username, user, 63);
                    break;
                }
            }
            pthread_mutex_unlock(&online_mutex);
            char update_query[256];
            sprintf(update_query, "UPDATE \"User\" SET status = 1 WHERE username = '%s';", user);
            PQexec(conn, update_query);

            send_line(fd, "OK LOGIN successful\n");
            
            friend_dao_get_list(fd, user);
            
            log_activity("LOGIN: %s is now online", user);
        } else {
            send_line(fd, "ERR Wrong username or password\n");
        }
    }

    else if (strcmp(cmd, "LOGOUT") == 0) {
        if (*is_logged_in) {
            char update_query[256];
            sprintf(update_query, "UPDATE \"User\" SET status = '0' WHERE username = '%s';", current_user);
            PQexec(conn, update_query);

            log_activity("LOGOUT: User %s has logged out", current_user);
            *is_logged_in = 0;
            memset(current_user, 0, 64);
            send_line(fd, "OK LOGOUT successful\n");
        } else {
            send_line(fd, "ERR You are not logged in\n");
        }
    }

    else if (strcmp(cmd, "LIST_FRIEND") == 0) {
        if (*is_logged_in) {
            friend_dao_get_list(fd, current_user);
        } else {
            send_line(fd, "ERR Please login first\n");
        }
    }

    else if (strcmp(cmd, "FRIEND_REQUEST") == 0) {
        if (*is_logged_in) {
            friend_dao_get_requests(fd, current_user);
        } else {
            send_line(fd, "ERR Please login first\n");
        }
    }

    else if (strcmp(cmd, "ADD_FRIEND") == 0) {
        if (*is_logged_in) {
            friend_dao_add_request(fd, current_user, user);
        } else {
            send_line(fd, "ERR Please login first\n");
        }
    }

    else if (strcmp(cmd, "ACCEPT_FRIEND") == 0) {
        if (*is_logged_in) {
            friend_dao_accept_request(fd, current_user, user);
        } else {
            send_line(fd, "ERR Please login first\n");
        }
    }

    else if (strcmp(cmd, "DECLINE_FRIEND") == 0) {
        if (*is_logged_in) {
            friend_dao_decline_request(fd, current_user, user);
        } else {
            send_line(fd, "ERR Please login first\n");
        }
    }

    else if (strcmp(cmd, "REMOVE_FRIEND") == 0) {
        if (*is_logged_in) {
            friend_dao_remove_friend(fd, current_user, user);
        } else {
            send_line(fd, "ERR Please login first\n");
        }
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "globals.h"
#include "dao/friend_dao.h"
#include "database.h"
#include "network.h"

void friend_dao_get_list(int client_fd, const char *username) {
    char query[1024];
    sprintf(query, 
        "SELECT f.friend_name, u.status FROM \"Friend\" f "
        "JOIN \"User\" u ON f.friend_name = u.username "
        "WHERE f.username = '%s' AND f.status = 'accepted';", username);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SQL Error: %s\n", PQerrorMessage(conn));
        send_line(client_fd, "ERR Database error while fetching friends\n");
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    if (rows == 0) {
        send_line(client_fd, "OK You have no friends in your list yet.\n");
    } else {
        send_line(client_fd, "--- DANH SÁCH BẠN BÈ ---\n");
        for (int i = 0; i < rows; i++) {
            char line[256];
            char *f_name = PQgetvalue(res, i, 0);
            char *f_status = PQgetvalue(res, i, 1);

            sprintf(line, "Friend: %s [%s]\n", f_name, 
                    (f_status[0] == '1') ? "ONLINE" : "OFFLINE");
            send_line(client_fd, line);
        }
        send_line(client_fd, "----------------------\n");
    }
    PQclear(res);
}

void friend_dao_add_request(int client_fd, const char *sender, const char *receiver) {
    char query[512];
    sprintf(query, "INSERT INTO \"Friend\" (username, friend_name, status) VALUES ('%s', '%s', 'pending');", 
            sender, receiver);
    
    PGresult *res = PQexec(conn, query);
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        send_line(client_fd, "ERR Lời mời đã gửi hoặc User không tồn tại\n");
    } else {
        send_line(client_fd, "OK Friend request sent\n");
        char notify_msg[256];
        sprintf(notify_msg, "NOTIFY: Bạn nhận được lời mời kết bạn từ %s\n", sender);
        notify_user(receiver, notify_msg); 
    }
    PQclear(res);
}

void friend_dao_accept_request(int client_fd, const char *username, const char *friend_name) {
    char query[1024];
    sprintf(query, 
        "BEGIN; "
        "UPDATE \"Friend\" SET status = 'accepted' WHERE username = '%s' AND friend_name = '%s'; "
        "INSERT INTO \"Friend\" (username, friend_name, status) VALUES ('%s', '%s', 'accepted') "
        "ON CONFLICT DO NOTHING; "
        "COMMIT;", friend_name, username, username, friend_name);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        send_line(client_fd, "ERR Chấp nhận thất bại\n");
    } else {
        send_line(client_fd, "OK Now you are friends\n");
        char notify_msg[256];
        sprintf(notify_msg, "NOTIFY: %s đã chấp nhận lời mời kết bạn của bạn!\n", username);
        notify_user(friend_name, notify_msg);
    }
    PQclear(res);
}

void friend_dao_decline_request(int client_fd, const char *my_username, const char *sender_name) {
    char query[512];
    sprintf(query, "DELETE FROM \"Friend\" WHERE username = '%s' AND friend_name = '%s' AND status = 'pending';", 
            sender_name, my_username);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        send_line(client_fd, "ERR Không thể từ chối lời mời.\n");
    } else {
        if (atoi(PQcmdTuples(res)) > 0) {
            send_line(client_fd, "OK Đã từ chối lời mời kết bạn.\n");
            char notify_msg[256];
            sprintf(notify_msg, "NOTIFY: %s đã từ chối lời mời kết bạn của bạn.\n", my_username);
            notify_user(sender_name, notify_msg);
        } else {
            send_line(client_fd, "ERR Không tìm thấy lời mời kết bạn tương ứng.\n");
        }
    }
    PQclear(res);
}

void friend_dao_remove_friend(int client_fd, const char *username, const char *friend_name) {
    char query[512];
    sprintf(query, "DELETE FROM \"Friend\" WHERE (username = '%s' AND friend_name = '%s') "
                   "OR (username = '%s' AND friend_name = '%s');", 
            username, friend_name, friend_name, username);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        send_line(client_fd, "ERR Lỗi khi hủy kết bạn\n");
    } else {
        send_line(client_fd, "OK Friend removed\n");
    }
    PQclear(res);
}

void friend_dao_get_requests(int client_fd, const char *username) {
    char query[512];
    sprintf(query, "SELECT username FROM \"Friend\" WHERE friend_name = '%s' AND status = 'pending';", username);
    
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        send_line(client_fd, "ERR Không thể lấy danh sách lời mời\n");
    } else {
        int rows = PQntuples(res);
        if (rows == 0) {
            send_line(client_fd, "OK Không có lời mời kết bạn nào.\n");
        } else {
            send_line(client_fd, "--- LỜI MỜI KẾT BẠN ĐANG CHỜ ---\n");
            for (int i = 0; i < rows; i++) {
                char buffer[256];
                sprintf(buffer, "User: %s gửi lời mời\n", PQgetvalue(res, i, 0));
                send_line(client_fd, buffer);
            }
        }
    }
    PQclear(res);
}


int friend_dao_are_friends(const char *user1, const char *user2) {
    if (!user1 || !user2 || strlen(user1) == 0 || strlen(user2) == 0) {
        return 0;
    }
    
    // Check if they are friends (bidirectional relationship)
    char query[512];
    snprintf(query, sizeof(query),
        "SELECT 1 FROM \"Friend\" "
        "WHERE (\"user1\" = '%s' AND \"user2\" = '%s') "
        "   OR (\"user1\" = '%s' AND \"user2\" = '%s') "
        "LIMIT 1;",
        user1, user2, user2, user1);
    
    PGresult *res = PQexec(conn, query);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SQL Error in friend_dao_are_friends: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }
    
    int are_friends = PQntuples(res) > 0 ? 1 : 0;
    PQclear(res);
    
    return are_friends;
}
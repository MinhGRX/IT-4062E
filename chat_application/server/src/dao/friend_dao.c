#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "globals.h"
#include "dao/friend_dao.h"
#include "services/log_service.h"
#include "database.h"
#include "network.h"

extern void log_activity(const char *fmt, ...);

void friend_dao_get_list(int client_fd, const char *username)
{
    char query[1024];
    sprintf(query,
            "SELECT f.friend_name, u.status FROM \"Friend\" f "
            "JOIN \"User\" u ON f.friend_name = u.username "
            "WHERE f.username = '%s' AND f.status = 'accepted';",
            username);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SQL Error: %s\n", PQerrorMessage(conn));
        send_line(client_fd, "ERR Database error while fetching friends\n");
        log_activity("[ERROR] FRIEND_DAO: Failed to get friend list for %s", username);
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    if (rows == 0)
    {
        send_line(client_fd, "OK You have no friends in your list yet.\n");
    }
    else
    {
        send_line(client_fd, "--- FRIEND LIST ---\n");
        for (int i = 0; i < rows; i++)
        {
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

void friend_dao_add_request(int client_fd, const char *sender, const char *receiver)
{
    char query[512];
    sprintf(query, "INSERT INTO \"Friend\" (username, friend_name, status) VALUES ('%s', '%s', 'pending');",
            sender, receiver);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        send_line(client_fd, "ERR Friend request already sent or user does not exist\n");
        log_activity("[ERROR] FRIEND_DAO: Failed to send friend request from %s to %s", sender, receiver);
    }
    else
    {
        send_line(client_fd, "OK Friend request sent\n");
        char notify_msg[256];
        sprintf(notify_msg, "NOTIFY: You received a friend request from %s\n", sender);
        notify_user(receiver, notify_msg);
    }
    PQclear(res);
}

void friend_dao_accept_request(int client_fd, const char *username, const char *friend_name)
{
    char query[1024];
    sprintf(query,
            "BEGIN; "
            "UPDATE \"Friend\" SET status = 'accepted' WHERE username = '%s' AND friend_name = '%s'; "
            "INSERT INTO \"Friend\" (username, friend_name, status) VALUES ('%s', '%s', 'accepted') "
            "ON CONFLICT DO NOTHING; "
            "COMMIT;",
            friend_name, username, username, friend_name);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        send_line(client_fd, "ERR Failed to accept friend request\n");
        log_activity("[ERROR] FRIEND_DAO: Failed to accept friend request from %s to %s", friend_name, username);
    }
    else
    {
        send_line(client_fd, "OK Now you are friends\n");
        char notify_msg[256];
        sprintf(notify_msg, "NOTIFY: %s has accepted your friend request!\n", username);
        notify_user(friend_name, notify_msg);
    }
    PQclear(res);
}

void friend_dao_decline_request(int client_fd, const char *my_username, const char *sender_name)
{
    char query[512];
    sprintf(query, "DELETE FROM \"Friend\" WHERE username = '%s' AND friend_name = '%s' AND status = 'pending';",
            sender_name, my_username);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        send_line(client_fd, "ERR Failed to decline friend request.\n");
        log_activity("[ERROR] FRIEND_DAO: Failed to decline friend request from %s to %s",
                     sender_name, my_username);
    }
    else
    {
        if (atoi(PQcmdTuples(res)) > 0)
        {
            send_line(client_fd, "OK Friend request declined.\n");
            char notify_msg[256];
            sprintf(notify_msg, "NOTIFY: %s has declined your friend request.\n", my_username);
            notify_user(sender_name, notify_msg);
        }
        else
        {
            send_line(client_fd, "ERR Cannot find suitable friend request.\n");
        }
    }
    PQclear(res);
}

void friend_dao_remove_friend(int client_fd, const char *username, const char *friend_name)
{
    char query[512];
    sprintf(query, "DELETE FROM \"Friend\" WHERE (username = '%s' AND friend_name = '%s') "
                   "OR (username = '%s' AND friend_name = '%s');",
            username, friend_name, friend_name, username);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        send_line(client_fd, "ERR Failed to remove friend\n");
        log_activity("[ERROR] FRIEND_DAO: Failed to remove friend %s for user %s",
                     friend_name, username);
    }
    else
    {
        send_line(client_fd, "OK Friend removed\n");
    }
    PQclear(res);
}

void friend_dao_get_requests(int client_fd, const char *username)
{
    char query[512];
    sprintf(query, "SELECT username FROM \"Friend\" WHERE friend_name = '%s' AND status = 'pending';", username);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        send_line(client_fd, "ERR Cannot get list friend request\n");
    }
    else
    {
        int rows = PQntuples(res);
        if (rows == 0)
        {
            send_line(client_fd, "OK No pending friend requests.\n");
        }
        else
        {
            send_line(client_fd, "--- PENDING FRIEND REQUESTS ---\n");
            for (int i = 0; i < rows; i++)
            {
                char buffer[256];
                sprintf(buffer, "User: %s sent a friend request\n", PQgetvalue(res, i, 0));
                send_line(client_fd, buffer);
            }
        }
    }
    PQclear(res);
}

int friend_dao_are_friends(const char *user1, const char *user2)
{
    if (!user1 || !user2 || strlen(user1) == 0 || strlen(user2) == 0)
    {
        return 0;
    }

    // Check if they are friends with status='accepted' (bidirectional)
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT 1 FROM \"Friend\" "
             "WHERE ((\"username\" = '%s' AND \"friend_name\" = '%s') "
             "    OR (\"username\" = '%s' AND \"friend_name\" = '%s')) "
             "  AND \"status\" = 'accepted' "
             "LIMIT 1;",
             user1, user2, user2, user1);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SQL Error in friend_dao_are_friends: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }

    int are_friends = PQntuples(res) > 0 ? 1 : 0;

    // Debug output
    printf("[DEBUG] Checking friendship: %s and %s -> %s\n",
           user1, user2, are_friends ? "YES" : "NO");

    PQclear(res);

    return are_friends;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "controllers/chat_controller.h"
#include "dao/chat_dao.h"
#include "network.h"
#include "globals.h"

// 1. Lưu tin nhắn
void chat_dao_save_message(const char *sender, const char *receiver, const char *content, int delivered)
{
    const char *query = "INSERT INTO \"MessageLog\" (sender, receiver, \"content\", sentTime, is_delivered) "
                        "VALUES ($1, $2, $3, CURRENT_TIMESTAMP, $4);";

    const char *paramValues[4];
    paramValues[0] = sender;
    paramValues[1] = receiver;
    paramValues[2] = content;
    paramValues[3] = delivered ? "TRUE" : "FALSE";

    PGresult *res = PQexecParams(conn, query, 4, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "[DAO] Save message failed: %s", PQerrorMessage(conn));
    }
    PQclear(res);
}

// 2. Lấy lịch sử tin nhắn
void chat_dao_get_history(int client_fd, const char *me, const char *friend_name)
{
    const char *query = "SELECT sender, \"content\", sentTime FROM \"MessageLog\" "
                        "WHERE (sender = $1 AND receiver = $2) OR (sender = $2 AND receiver = $1) "
                        "ORDER BY sentTime ASC LIMIT 50;";

    const char *params[2] = {me, friend_name};
    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int rows = PQntuples(res);
        send_line(client_fd, "\n--- CHAT HISTORY ---\n");
        if (rows == 0)
        {
            send_line(client_fd, "(No messages yet)\n");
        }
        else
        {
            for (int i = 0; i < rows; i++)
            {
                char buf[BUF_SIZE];
                snprintf(buf, sizeof(buf), "[%s] %s: %s\n",
                         PQgetvalue(res, i, 2), PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
                send_line(client_fd, buf);
            }
        }
        send_line(client_fd, "--- END OF HISTORY ---\n\n");
    }
    PQclear(res);
}

// 3. Đánh dấu đã đọc
void chat_dao_mark_delivered(const char *me, const char *friend_name)
{
    const char *query = "UPDATE \"MessageLog\" SET is_delivered = TRUE "
                        "WHERE sender = $1 AND receiver = $2 AND is_delivered = FALSE;";

    const char *params[2] = {friend_name, me};
    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "[DAO] Update status failed: %s", PQerrorMessage(conn));
    }
    PQclear(res);
}

// 4. Lấy danh sách thông báo tin nhắn chờ (Dùng khi Login)
void chat_dao_get_pending_senders(int client_fd, const char *me)
{
    const char *query = "SELECT DISTINCT sender FROM \"MessageLog\" "
                        "WHERE receiver = $1 AND is_delivered = FALSE;";

    const char *params[1] = {me};
    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int rows = PQntuples(res);
        if (rows > 0)
        {
            char list[1024] = "NOTIFY_PENDING: ";
            for (int i = 0; i < rows; i++)
            {
                strncat(list, PQgetvalue(res, i, 0), sizeof(list) - strlen(list) - 1);
                if (i < rows - 1)
                    strncat(list, ", ", sizeof(list) - strlen(list) - 1);
            }
            strncat(list, " has sent you a message.\n", sizeof(list) - strlen(list) - 1);
            send_line(client_fd, list);
        }
    }
    PQclear(res);
}
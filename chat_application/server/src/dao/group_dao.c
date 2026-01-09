#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "dao/group_dao.h"
#include "network.h"
#include <globals.h>

int group_dao_create(const char *group_name, const char *creator, int *out_group_id)
{
    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO \"Group\" (\"groupName\", \"creator\") VALUES ('%s', '%s') RETURNING \"groupId\";",
             group_name, creator);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SQL Error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    *out_group_id = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    // Add creator as owner
    return group_dao_add_member(*out_group_id, creator, "owner");
}

int group_dao_add_member(int group_id, const char *username, const char *role)
{
    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO \"GroupMember\" (\"groupId\", \"username\", \"role\") VALUES (%d, '%s', '%s') ON CONFLICT DO NOTHING;",
             group_id, username, role);

    PGresult *res = PQexec(conn, query);

    int ok = PQresultStatus(res) == PGRES_COMMAND_OK;
    PQclear(res);
    return ok ? 0 : -1;
}

int group_dao_is_member(int group_id, const char *username)
{
    if (!username || strlen(username) == 0)
    {
        fprintf(stderr, "ERROR: Invalid username\n");
        return 0;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT 1 FROM \"GroupMember\" WHERE \"groupId\" = %d AND \"username\" = '%s';",
             group_id, username);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SQL Error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }

    int is_member = PQntuples(res) > 0 ? 1 : 0;
    PQclear(res);

    return is_member;
}

int group_dao_is_owner(int group_id, const char *username)
{
    if (!username || strlen(username) == 0)
    {
        fprintf(stderr, "ERROR: Invalid username\n");
        return 0;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT 1 FROM \"GroupMember\" "
             "WHERE \"groupId\" = %d AND \"username\" = '%s' AND \"role\" = 'owner';",
             group_id, username);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SQL Error in group_dao_is_owner: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }

    int is_owner = PQntuples(res) > 0 ? 1 : 0;
    PQclear(res);

    return is_owner;
}

int group_dao_remove_member(int group_id, const char *username)
{
    if (!username || strlen(username) == 0)
    {
        fprintf(stderr, "ERROR: Invalid username\n");
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "DELETE FROM \"GroupMember\" "
             "WHERE \"groupId\" = %d AND \"username\" = '%s';",
             group_id, username);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "SQL Error in group_dao_remove_member: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    PQclear(res);
    return 0;
}

int group_dao_save_message(int group_id, const char *sender, const char *message)
{
    if (!sender || !message || strlen(sender) == 0 || strlen(message) == 0)
    {
        fprintf(stderr, "ERROR: Invalid sender or message\n");
        return -1;
    }

    // Escape single quotes in message to prevent SQL injection
    char escaped_message[4096];
    int j = 0;
    for (int i = 0; message[i] != '\0' && j < sizeof(escaped_message) - 2; i++)
    {
        if (message[i] == '\'')
        {
            escaped_message[j++] = '\'';
            escaped_message[j++] = '\'';
        }
        else
        {
            escaped_message[j++] = message[i];
        }
    }
    escaped_message[j] = '\0';

    char query[4096];
    snprintf(query, sizeof(query),
             "INSERT INTO \"GroupMessage\" (\"groupId\", \"sender\", \"message\", \"timestamp\") "
             "VALUES (%d, '%s', '%s', NOW());",
             group_id, sender, escaped_message);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "SQL Error in group_dao_save_message: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    PQclear(res);
    return 0;
}

int group_dao_get_history(int group_id, int limit, char ***out_messages, int *out_count)
{
    if (!out_messages || !out_count)
    {
        fprintf(stderr, "ERROR: Invalid output parameters\n");
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT \"sender\", \"message\", \"timestamp\" "
             "FROM \"GroupMessage\" "
             "WHERE \"groupId\" = %d "
             "ORDER BY \"timestamp\" DESC "
             "LIMIT %d;",
             group_id, limit);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SQL Error in group_dao_get_history: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    int rows = PQntuples(res);
    *out_count = rows;

    if (rows == 0)
    {
        *out_messages = NULL;
        PQclear(res);
        return 0;
    }

    // Allocate array of message strings
    *out_messages = (char **)malloc(rows * sizeof(char *));
    if (!*out_messages)
    {
        fprintf(stderr, "ERROR: Memory allocation failed\n");
        PQclear(res);
        return -1;
    }

    // Format: "[timestamp] sender: message"
    for (int i = 0; i < rows; i++)
    {
        const char *sender = PQgetvalue(res, i, 0);
        const char *message = PQgetvalue(res, i, 1);
        const char *timestamp = PQgetvalue(res, i, 2);

        // Allocate space for formatted message (timestamp is ~19 chars)
        size_t msg_len = strlen(sender) + strlen(message) + strlen(timestamp) + 50;
        (*out_messages)[i] = (char *)malloc(msg_len);

        if ((*out_messages)[i])
        {
            snprintf((*out_messages)[i], msg_len, "[%s] %s: %s", timestamp, sender, message);
        }
    }

    PQclear(res);
    return 0;
}

int group_dao_get_members(int group_id, char ***out_members, int *out_count)
{
    if (!out_members || !out_count)
    {
        fprintf(stderr, "ERROR: Invalid output parameters\n");
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT \"username\" FROM \"GroupMember\" "
             "WHERE \"groupId\" = %d;",
             group_id);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SQL Error in group_dao_get_members: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    int rows = PQntuples(res);
    *out_count = rows;

    if (rows == 0)
    {
        *out_members = NULL;
        PQclear(res);
        return 0;
    }

    *out_members = (char **)malloc(rows * sizeof(char *));
    if (!*out_members)
    {
        fprintf(stderr, "ERROR: Memory allocation failed\n");
        PQclear(res);
        return -1;
    }

    for (int i = 0; i < rows; i++)
    {
        const char *username = PQgetvalue(res, i, 0);
        (*out_members)[i] = strdup(username);
        if (!(*out_members)[i])
        { // Error handling added
            fprintf(stderr, "ERROR: Memory allocation failed for username\n");
            // Cleanup already allocated members
            for (int j = 0; j < i; j++)
            {
                free((*out_members)[j]);
            }
            free(*out_members);
            PQclear(res);
            return -1;
        }
    }
    PQclear(res);
    return 0;
}

int group_dao_list_user_groups(const char *username, char ***out_groups, int *out_count) {
    if (!out_groups || !out_count) {
        fprintf(stderr, "ERROR: Invalid output parameters\n");
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT g.\"groupName\" FROM \"Group\" g "
        "JOIN \"GroupMember\" gm ON g.\"groupId\" = gm.\"groupId\" "
        "WHERE gm.\"username\" = '%s';",
        username);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SQL Error in group_dao_list_user_groups: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    int rows = PQntuples(res);
    *out_count = rows;

    if (rows == 0) {
        *out_groups = NULL;
        PQclear(res);
        return 0;
    }

    *out_groups = (char **)malloc(rows * sizeof(char *));
    if (!*out_groups) {
        fprintf(stderr, "ERROR: Memory allocation failed\n");
        PQclear(res);
        return -1;
    }

    for (int i = 0; i < rows; i++) {
        const char *group_name = PQgetvalue(res, i, 0);
        (*out_groups)[i] = strdup(group_name);
    }

    PQclear(res);
    return 0;
}
#ifndef GLOBALS_H
#define GLOBALS_H

#include <pthread.h>

#define MAX_CLIENTS 100
#define BUF_SIZE 4096
typedef struct {
    char username[BUF_SIZE];
    int fd;
    int is_logged_in;
    char chatting_with[BUF_SIZE];
    int current_group_id;          
    char current_group_name[BUF_SIZE];
} ActiveUser;

extern ActiveUser online_users[MAX_CLIENTS];
extern pthread_mutex_t online_mutex;

int get_user_index_by_fd(int fd);
void notify_user(const char *target_username, const char *message);

#endif
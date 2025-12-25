#ifndef GLOBALS_H
#define GLOBALS_H

#include <pthread.h>

#define MAX_CLIENTS 100

typedef struct {
    int fd;
    char username[64];
} ActiveUser;

extern ActiveUser online_users[MAX_CLIENTS];
extern pthread_mutex_t online_mutex;

// Khai báo prototype hàm notify để file DAO gọi được
void notify_user(const char *target_username, const char *msg);

#endif
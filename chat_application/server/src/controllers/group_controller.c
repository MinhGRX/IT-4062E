#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "../../include/controllers/group_controller.h"
#include "../../include/dao/group_dao.h"
#include "../../include/network.h"
#include "../../include/globals.h"

extern ActiveUser online_users[MAX_CLIENTS];
extern pthread_mutex_t online_mutex;


void group_controller_create(int client_fd, char *username, const char *group_name) {
    int gid = 0;
    if (group_dao_create(group_name, username, &gid) == 0) {
        char ok[128];
        snprintf(ok, sizeof(ok), "OK Group created successfully (ID:%d)\n", gid);
        send_line(client_fd, ok);
    } else {
        send_line(client_fd, "ERR Group creation failed (name may exist)\n");
    }
}


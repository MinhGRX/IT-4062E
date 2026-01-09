#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H
#include <string.h>
void auth_controller_handle(int fd, char *cmd, char *user, char *pass,
                            char *current_user, int *is_logged_in);

#endif
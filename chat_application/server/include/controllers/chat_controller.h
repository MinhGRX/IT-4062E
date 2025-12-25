#ifndef CHAT_CONTROLLER_H
#define CHAT_CONTROLLER_H

#include "../globals.h"

void chat_controller_enter(int client_idx, const char *target);
void chat_controller_send(int client_idx, const char *content);
void chat_controller_leave(int client_idx);
void chat_controller_handle_line(int client_idx, char *line);

#endif
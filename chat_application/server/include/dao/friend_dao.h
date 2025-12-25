#ifndef FRIEND_DAO_H
#define FRIEND_DAO_H

void friend_dao_get_list(int client_fd, const char *username);
void friend_dao_add_request(int client_fd, const char *sender, const char *receiver);
void friend_dao_accept_request(int client_fd, const char *username, const char *friend_name);
void friend_dao_remove_friend(int client_fd, const char *username, const char *friend_name);
void friend_dao_get_requests(int client_fd, const char* username);
void friend_dao_decline_request(int client_fd, const char *username, const char *friend_name);

#endif
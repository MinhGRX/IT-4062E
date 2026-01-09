#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

// Hàm kết nối tới server
int connect_to_server(const char *host, const char *port);

// Luồng chạy ngầm để liên tục nhận tin nhắn (Point 1 in your rubric)
void *start_receive_loop(void *socket_fd);

// Hàm gửi tin nhắn có format \n
int send_command(int fd, const char *cmd);

#endif
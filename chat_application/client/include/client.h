#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUF_SIZE 4096

// Hàm gửi tin nhắn kèm \n (Xử lý truyền dòng - 1đ)
int send_line(int fd, const char *msg);

// Hàm luồng phụ để nhận tin nhắn từ server (Cơ chế I/O - 2đ)
static void* receive_handler(void* socket_desc);

#endif
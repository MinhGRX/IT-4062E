#include "client.h"

void* receive_handler(void* socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[BUF_SIZE];
    ssize_t n;

    while (1) {
        memset(buffer, 0, BUF_SIZE);
        n = recv(sock, buffer, BUF_SIZE - 1, 0);
        
        if (n > 0) {
            printf("\n[SERVER]: %s", buffer);
            printf("\n> "); 
            fflush(stdout);
        } else if (n == 0) {
            printf("\n[CLIENT] Mất kết nối tới Server.\n");
            exit(0);
        } else {
            perror("Lỗi nhận tin nhắn");
            exit(1);
        }
    }
    return NULL;
}

int send_line(int fd, const char *msg) {
    return send(fd, msg, strlen(msg), 0) < 0 ? -1 : 0;
}
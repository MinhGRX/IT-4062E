#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 4096

static int send_line(int fd, const char *msg)
{
    size_t len = strlen(msg);
    if (send(fd, msg, len, 0) < 0)
        return -1;
    return 0;
}

// Drain any pending server messages (e.g., auto friend list after login)
static void drain_pending_messages(int sock)
{
    char buffer[BUF_SIZE];
    fd_set rfds;
    struct timeval tv = {0, 200000}; // 200 ms timeout

    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
        int r = select(sock + 1, &rfds, NULL, NULL, &tv);
        if (r <= 0)
            break; // timeout or error

        int n = recv(sock, buffer, BUF_SIZE - 1, 0);
        if (n <= 0)
            break;
        buffer[n] = '\0';
        printf("%s", buffer); // print raw, do not truncate at newline

        // After first read, switch to zero-timeout to drain quickly
        tv.tv_sec = 0;
        tv.tv_usec = 0;
    }
}

static void *receive_handler(void *socket_desc)
{
    int sock = *(int *)socket_desc;
    char buffer[BUF_SIZE];

    while (1)
    {
        memset(buffer, 0, BUF_SIZE);
        int n = recv(sock, buffer, BUF_SIZE - 1, 0);
        if (n > 0)
        {
            buffer[n] = '\0';
            if (strncmp(buffer, "NOTIFY:", 7) == 0)
            {
                printf("\n[NOTIFY]: %s", buffer + 7);
            }
            else
            {
                printf("%s", buffer); // print raw
                if (strstr(buffer, "OK LOGIN successful") != NULL)
                {
                    drain_pending_messages(sock);
                }
            }
            printf("> ");
            fflush(stdout);
        }
        else if (n == 0)
        {
            printf("\n[CLIENT] Server closed connection.\n");
            exit(0);
        }
        else
        {
            break;
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("[CLIENT] Cách dùng: %s <host> <port>\n", argv[0]);
        return 1;
    }

    // 1. Giải quyết địa chỉ IP/Domain
    struct addrinfo hints, *pResult;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(argv[1], argv[2], &hints, &pResult) != 0)
    {
        perror("Lỗi getaddrinfo");
        return 1;
    }

    // 2. Tạo Socket và kết nối
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(s, pResult->ai_addr, pResult->ai_addrlen) != 0)
    {
        printf("[CLIENT] Can't connect to Server at %s:%s\n", argv[1], argv[2]);
        freeaddrinfo(pResult);
        return 1;
    }
    freeaddrinfo(pResult);
    printf("[CLIENT] Connection successfully!\n");

    // 3. KHỞI TẠO LUỒNG NHẬN TIN NHẮN (Ghi điểm cơ chế Bất đồng bộ)
    pthread_t tid;
    if (pthread_create(&tid, NULL, receive_handler, (void *)&s) != 0)
    {
        perror("Không thể tạo luồng nhận");
        return 1;
    }
    pthread_detach(tid);

    // 4. LUỒNG CHÍNH: Vòng lặp nhập lệnh từ bàn phím
    char input[BUF_SIZE];
    while (1)
    {
        printf("> ");
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) != NULL)
        {
            if (strcmp(input, "/quit\n") == 0)
            {
                printf("[CLIENT] Exiting...\n");
                break;
            }
            send_line(s, input);
        }
    }

    close(s);
    return 0;
}
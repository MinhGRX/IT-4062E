// Based on simple_tcp_client.c from References
// Modified to add: newline-framed command sending

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 4096

static int send_line(int fd, const char *msg) {
    size_t len = strlen(msg);
    if (send(fd, msg, len, 0) < 0) {
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc >= 3)
    {
        struct addrinfo *pResult = NULL;
        struct addrinfo *pTmp = NULL;
        const char *domain = argv[1];
        const char *service = argv[2];
        
        getaddrinfo(domain, service, NULL, &pResult);
        pTmp = pResult;
        
        while (pTmp != NULL)
        {
            if (pTmp->ai_family == AF_INET)
            {
                struct sockaddr_in *pAddr = (struct sockaddr_in *)pTmp->ai_addr;
                char *ipStr = inet_ntoa(pAddr->sin_addr);
                printf("[CLIENT] Resolved %s:%s to %s:%d\n", domain, service, ipStr, ntohs(pAddr->sin_port));
                break;
            }
            pTmp = pTmp->ai_next;
        }

        int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s > 0)
        {
            int error = connect(s, pTmp->ai_addr, pTmp->ai_addrlen);
            if (error == 0)
            {
                printf("[CLIENT] Connected!\n");

                char buffer[BUF_SIZE];
                memset(buffer, 0, sizeof(buffer));
                recv(s, buffer, sizeof(buffer) - 1, 0);
                printf("%s", buffer);

                for (;;)
                {
                    printf("> ");
                    fflush(stdout);

                    char input[BUF_SIZE];
                    memset(input, 0, sizeof(input));
                    if (fgets(input, sizeof(input), stdin) != NULL)
                    {
                        size_t len = strlen(input);
                        if (len > 0 && input[len-1] != '\n')
                        {
                            if (len + 1 < sizeof(input))
                            {
                                input[len] = '\n';
                                input[len+1] = '\0';
                            }
                        }

                        if (strcmp(input, "/quit\n") == 0)
                        {
                            printf("[CLIENT] Exiting.\n");
                            break;
                        }

                        if (send_line(s, input) == 0)
                        {
                            memset(buffer, 0, sizeof(buffer));
                            ssize_t n = recv(s, buffer, sizeof(buffer) - 1, 0);
                            if (n > 0)
                            {
                                printf("%s", buffer);
                            }
                            else
                            {
                                printf("[CLIENT] Server disconnected.\n");
                                break;
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                close(s);
            }
            else
            {
                printf("[CLIENT] Failed to connect\n");
                close(s);
            }
        }

        if (pResult != NULL)
        {
            freeaddrinfo(pResult);
        }
    }
    else
    {
        printf("[CLIENT] Usage: %s <domain> <service>\n", argv[0]);
        printf("[CLIENT] Example: %s localhost 9090\n", argv[0]);
    }

    return 0;
}
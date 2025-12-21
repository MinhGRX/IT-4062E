// Based on multithread_tcp_chatroom_server.c from References
// Modified to add: line-framed command parsing, handle_line() stub

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>

#define BUF_SIZE 4096
#define BACKLOG  128
#define DATA_DIR "./data"
#define LOG_DIR  "./logs"
#define USERS_FILE "./data/users.txt"
#define LOG_FILE "./logs/server.log"

static pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

static void rstrip(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r'))
    {
        s[--len] = '\0';
    }
}

static int send_line(int fd, const char *msg)
{
    size_t len = strlen(msg);
    if (send(fd, msg, len, 0) < 0)
    {
        return -1;
    }
    return 0;
}

static void ensure_dirs(void)
{
    mkdir(DATA_DIR, 0755);
    mkdir(LOG_DIR, 0755);
}

static void log_activity(const char *fmt, ...)
{
    pthread_mutex_lock(&file_mutex);
    
    FILE *f = fopen(LOG_FILE, "a");
    if (f != NULL)
    {
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        char ts[64];
        strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", tm_info);
        
        fprintf(f, "[%s] ", ts);
        
        va_list ap;
        va_start(ap, fmt);
        vfprintf(f, fmt, ap);
        va_end(ap);
        
        fprintf(f, "\n");
        fclose(f);
    }
    
    pthread_mutex_unlock(&file_mutex);
}

static int user_exists(const char *username)
{
    pthread_mutex_lock(&file_mutex);
    
    FILE *f = fopen(USERS_FILE, "r");
    int found = 0;
    
    if (f != NULL)
    {
        char line[BUF_SIZE];
        while (fgets(line, sizeof(line), f) != NULL)
        {
            rstrip(line);
            char *colon = strchr(line, ':');
            if (colon != NULL)
            {
                *colon = '\0';
                if (strcmp(line, username) == 0)
                {
                    found = 1;
                    break;
                }
            }
        }
        fclose(f);
    }
    
    pthread_mutex_unlock(&file_mutex);
    return found;
}

static int check_password(const char *username, const char *password)
{
    pthread_mutex_lock(&file_mutex);
    
    FILE *f = fopen(USERS_FILE, "r");
    int match = 0;
    
    if (f != NULL)
    {
        char line[BUF_SIZE];
        while (fgets(line, sizeof(line), f) != NULL)
        {
            rstrip(line);
            char *colon = strchr(line, ':');
            if (colon != NULL)
            {
                char *stored_user = line;
                char *stored_pass = colon + 1;
                *colon = '\0';
                
                if (strcmp(stored_user, username) == 0 && strcmp(stored_pass, password) == 0)
                {
                    match = 1;
                    break;
                }
            }
        }
        fclose(f);
    }
    
    pthread_mutex_unlock(&file_mutex);
    return match;
}

static int register_user(const char *username, const char *password)
{
    pthread_mutex_lock(&file_mutex);
    
    int result = 0;
    FILE *f = fopen(USERS_FILE, "a");
    if (f != NULL)
    {
        fprintf(f, "%s:%s\n", username, password);
        fclose(f);
        result = 1;
    }
    
    pthread_mutex_unlock(&file_mutex);
    return result;
}

static void handle_command(int fd, const char *line, char *username, int *logged_in)
{
    char response[BUF_SIZE];
    char *copy = malloc(strlen(line) + 1);
    if (copy == NULL)
    {
        return;
    }
    strcpy(copy, line);
    
    char *cmd = strtok(copy, " ");
    
    if (cmd == NULL)
    {
        free(copy);
        return;
    }
    
    if (strcmp(cmd, "REGISTER") == 0)
    {
        char *user = strtok(NULL, " ");
        char *pass = strtok(NULL, " ");
        
        if (user == NULL || pass == NULL)
        {
            send_line(fd, "ERR Invalid REGISTER syntax\n");
        }
        else if (user_exists(user))
        {
            send_line(fd, "ERR User already exists\n");
        }
        else if (register_user(user, pass))
        {
            send_line(fd, "OK User registered\n");
            log_activity("REGISTER %s", user);
        }
        else
        {
            send_line(fd, "ERR Registration failed\n");
        }
    }
    else if (strcmp(cmd, "LOGIN") == 0)
    {
        char *user = strtok(NULL, " ");
        char *pass = strtok(NULL, " ");
        
        if (user == NULL || pass == NULL)
        {
            send_line(fd, "ERR Invalid LOGIN syntax\n");
        }
        else if (!user_exists(user))
        {
            send_line(fd, "ERR User not found\n");
        }
        else if (!check_password(user, pass))
        {
            send_line(fd, "ERR Wrong password\n");
        }
        else
        {
            strncpy(username, user, 63);
            username[63] = '\0';
            *logged_in = 1;
            send_line(fd, "OK LOGIN successful\n");
            log_activity("LOGIN %s", user);
        }
    }
    else if (strcmp(cmd, "LOGOUT") == 0)
    {
        if (*logged_in)
        {
            log_activity("LOGOUT %s", username);
            send_line(fd, "OK LOGOUT successful\n");
            *logged_in = 0;
            username[0] = '\0';
        }
        else
        {
            send_line(fd, "ERR Not logged in\n");
        }
    }
    else if (strcmp(cmd, "PING") == 0)
    {
        send_line(fd, "PONG\n");
    }
    else
    {
        snprintf(response, sizeof(response), "OK %s\n", line);
        send_line(fd, response);
    }
    
    free(copy);
}

static void *do_client(void *arg)
{
    int cfd = *(int *)arg;
    free(arg);

    char username[64];
    username[0] = '\0';
    int logged_in = 0;

    char inbuf[BUF_SIZE];
    size_t inlen = 0;

    send_line(cfd, "OK HELLO\n");

    for (;;)
    {
        char buf[BUF_SIZE];
        memset(buf, 0, sizeof(buf));
        ssize_t n = recv(cfd, buf, sizeof(buf) - 1, 0);
        
        if (n <= 0)
        {
            if (logged_in)
            {
                log_activity("DISCONNECT %s", username);
            }
            break;
        }

        if (inlen + (size_t)n > sizeof(inbuf))
        {
            inlen = 0;
            send_line(cfd, "ERR Line too long\n");
            continue;
        }

        memcpy(inbuf + inlen, buf, (size_t)n);
        inlen += (size_t)n;

        size_t start = 0;
        for (size_t i = 0; i < inlen; i++)
        {
            if (inbuf[i] == '\n')
            {
                size_t line_len = i - start + 1;
                if (line_len >= BUF_SIZE)
                {
                    start = i + 1;
                    continue;
                }

                char line[BUF_SIZE];
                memset(line, 0, sizeof(line));
                memcpy(line, inbuf + start, line_len);
                line[line_len] = '\0';
                rstrip(line);
                
                if (line[0] != '\0')
                {
                    handle_command(cfd, line, username, &logged_in);
                }

                start = i + 1;
            }
        }

        if (start > 0)
        {
            memmove(inbuf, inbuf + start, inlen - start);
            inlen -= start;
        }
    }

    close(cfd);
    return NULL;
}

int main(int argc, char **argv)
{
    ensure_dirs();
    log_activity("SERVER START");

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
                printf("[SERVER] Resolved %s:%s to %s:%d\n", domain, service, ipStr, ntohs(pAddr->sin_port));
                break;
            }
            pTmp = pTmp->ai_next;
        }

        int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_fd > 0)
        {
            int opt = 1;
            setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            if (bind(server_fd, pTmp->ai_addr, pTmp->ai_addrlen) == 0)
            {
                if (listen(server_fd, BACKLOG) == 0)
                {
                    printf("[SERVER] Listening on %s:%s\n", domain, service);

                    while (1)
                    {
                        struct sockaddr_in client_addr;
                        socklen_t client_len = sizeof(client_addr);

                        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
                        if (client_fd > 0)
                        {
                            printf("[SERVER] Client accepted from %s:%d\n",
                                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                            int *pfd = malloc(sizeof(int));
                            if (pfd != NULL)
                            {
                                *pfd = client_fd;

                                pthread_t tid;
                                if (pthread_create(&tid, NULL, do_client, pfd) == 0)
                                {
                                    pthread_detach(tid);
                                }
                                else
                                {
                                    close(client_fd);
                                    free(pfd);
                                }
                            }
                            else
                            {
                                close(client_fd);
                            }
                        }
                    }
                }
            }

            close(server_fd);
        }

        if (pResult != NULL)
        {
            freeaddrinfo(pResult);
        }
    }
    else
    {
        printf("[SERVER] Usage: %s <domain> <service>\n", argv[0]);
        printf("[SERVER] Example: %s localhost 9090\n", argv[0]);
    }

    return 0;
}
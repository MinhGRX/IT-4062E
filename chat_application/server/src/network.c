#include <string.h>
#include <sys/socket.h>
#include "network.h"

int send_line(int fd, const char *msg)
{
    size_t len = strlen(msg);
    if (send(fd, msg, len, 0) < 0)
    {
        return -1;
    }
    return 0;
}
#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>

/**
 * Gửi một chuỗi ký tự qua socket, đảm bảo kết thúc bằng \n
 */
int send_line(int fd, const char *msg);

#endif
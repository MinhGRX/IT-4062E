#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H
#include <string.h>
/**
 * Tiếp nhận các tham số đã tách từ chuỗi lệnh và gọi Service tương ứng.
 * fd: socket của client
 * cmd: lệnh (REGISTER/LOGIN/LOGOUT)
 * user, pass: thông tin từ client
 * current_user: buffer để lưu tên user hiện tại của session
 * is_logged_in: cờ trạng thái đăng nhập của session
 */
void auth_controller_handle(int fd, char *cmd, char *user, char *pass, char *current_user, int *is_logged_in);

#endif
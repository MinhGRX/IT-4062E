#ifndef USER_SERVICE_H
#define USER_SERVICE_H

/**
 * Xử lý đăng ký: Kiểm tra tồn tại, độ dài mật khẩu...
 * Trả về: 1 (Thành công), -1 (User tồn tại), -2 (Lỗi logic khác).
 */
int user_service_register(const char *username, const char *password);

/**
 * Xử lý đăng nhập: Kiểm tra thông tin và có thể cập nhật trạng thái online.
 * Trả về: 1 (Thành công), 0 (Thất bại).
 */
int user_service_login(const char *username, const char *password);

#endif
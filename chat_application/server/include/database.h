#ifndef DATABASE_H
#define DATABASE_H

/* * Thư viện libpq-fe.h là thư viện lập trình C chính thức của PostgreSQL.
 * Đảm bảo bạn đã cài đặt libpq-dev trên WSL: 
 * sudo apt install libpq-dev
 */
#include <libpq-fe.h>

/**
 * Biến toàn cục đại diện cho kết nối tới Database.
 * 'extern' cho phép biến này được truy cập từ mọi file .c khác (như main.c, user_dao.c).
 */
extern PGconn *conn;

/**
 * Hàm thiết lập kết nối tới PostgreSQL.
 * Nếu kết nối thất bại, hàm sẽ in lỗi và thoát chương trình.
 */
void db_connect();

/**
 * Hàm đóng kết nối và giải phóng bộ nhớ.
 * Nên gọi hàm này trước khi Server tắt hẳn.
 */
void db_disconnect();

#endif /* DATABASE_H */
package server.service;

import server.repositories.UserInfo.UserInfoRepository;

import java.sql.SQLException;

public class LogoutService {
    private static final LogoutService logoutService = new LogoutService();
    private final UserInfoRepository userRepository;

    private LogoutService() {
        this.userRepository = UserInfoRepository.getInstance();
    }

    public static LogoutService getInstance() {
        return logoutService;
    }

    public String response(String username) throws SQLException {
        userRepository.updateUserStatus(username, "0");
        return "Type: Logout;Message: 1";
    }
}
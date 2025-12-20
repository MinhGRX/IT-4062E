package server.service;

import server.repositories.UserInfo.UserInfoRepository;

import java.sql.SQLException;

public class CheckOnlineService {
    private static final CheckOnlineService CHECK_ONLINE_SERVICE = new CheckOnlineService();
    private final UserInfoRepository userRepository;

    private CheckOnlineService() {
        this.userRepository = UserInfoRepository.getInstance();
    }

    public static CheckOnlineService getInstance() {
        return CHECK_ONLINE_SERVICE;
    }

    public boolean isUserOnline(String username) throws SQLException {
        String status = userRepository.getUserStatus(username);
        return "1".equals(status); // Return true if the user is online
    }

    public String response(String username) throws SQLException {
        if (isUserOnline(username)) {
            return "Type: checkOnline;Message: 1";
        } else {
            return "Type: logOut;Message: time out";
        }
    }
}

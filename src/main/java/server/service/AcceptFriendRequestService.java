package server.service;

import server.repositories.UserFriend.UserFriendListRepository;

import java.sql.SQLException;

public class AcceptFriendRequestService {
    private final String type = "AcceptFriendRequest";
    private static final AcceptFriendRequestService acceptFriendRequestService = new AcceptFriendRequestService();
    private UserFriendListRepository userFriendListRepository = UserFriendListRepository.getInstance();
    private String serverResponse;

    public String getServerResponse() {
        return serverResponse;
    }

    private AcceptFriendRequestService() {
    }

    public String response(String user, String to, String status) throws SQLException {
        serverResponse = acceptFriendRequest(user, to, status);
        return "Type: " + type + "; Status: " + serverResponse;
    }

    public static AcceptFriendRequestService getInstance() {
        return acceptFriendRequestService;
    }

    private String acceptFriendRequest(String username, String to, String status) throws SQLException {
        if (status.equals("1")) {
            // Accept the friend request
            userFriendListRepository.acceptFriendRequest(username, to);
            return "1"; // Request accepted
        } else {
            // Deny the friend request
            userFriendListRepository.deleteFriendRequest(username, to);
            return "0"; // Request denied
        }
    }
}

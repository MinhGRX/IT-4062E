package server.service;

import server.repositories.UserFriend.UserFriendListRepository;

import java.sql.SQLException;

public class SendFriendRequestService {
    private final String type = "SendFriendRequest";
    private static final SendFriendRequestService sendFriendRequestService = new SendFriendRequestService();
    private UserFriendListRepository userFriendListRepository = UserFriendListRepository.getInstance();
    private String serverResponse;

    public String getServerResponse() {
        return serverResponse;
    }

    private SendFriendRequestService() {
    }

    public String response(String sender, String receiver) throws SQLException {
        serverResponse = sendFriendRequest(sender, receiver);
        return "Type: " + type + "; Status: " + serverResponse;
    }

    public static SendFriendRequestService getInstance() {
        return sendFriendRequestService;
    }

    private String sendFriendRequest(String sender, String receiver) throws SQLException {
        if (sender.equals(receiver)) {
            return "0"; // Cannot send request to self
        }

        if (userFriendListRepository.isFriendRequestPending(sender, receiver)) {
            return "0"; // Already a pending request
        }

        if (userFriendListRepository.getFriendList(sender).contains(receiver)) {
            return "0"; // Already friends
        }

        String requestTime = ""; // No need to use current time

        // Send the friend request as pending
        userFriendListRepository.addFriendRequest(sender, receiver, "pending");
        return "1"; // Friend request sent successfully
    }
}

package server.service;

import server.repositories.UserFriend.UserFriendListRepository;

import java.sql.SQLException;
import java.util.List;
import java.util.Map;

public class ListFriendService {
    private static final ListFriendService listFriendService = new ListFriendService();
    private final String type = "ViewListFriend";
    private final UserFriendListRepository userFriendListRepository;

    private ListFriendService() {
        this.userFriendListRepository = UserFriendListRepository.getInstance();
    }

    public static ListFriendService getInstance() {
        return listFriendService;
    }

    public String response(String username) throws SQLException {
        // Fetch friend list along with status from the repository
        List<Map<String, String>> friendList = userFriendListRepository.getFriendListWithStatus(username);
        StringBuilder responseBuilder = new StringBuilder("Type: " + type + ";listFriend: ");

        // Append each friend and their status to the response
        for (Map<String, String> friend : friendList) {
            String friendName = friend.get("friendName");
            String status = friend.get("status"); // Status is directly included as '1' or '0'
            responseBuilder.append(friendName).append("(").append(status).append("),");
        }

        // Remove trailing comma, if present
        if (responseBuilder.length() > 0 && responseBuilder.charAt(responseBuilder.length() - 1) == ',') {
            responseBuilder.setLength(responseBuilder.length() - 1);
        }

        return responseBuilder.toString();
    }
}

package server.repositories.UserFriend;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static server.database.database.connection;

public class UserFriendListRepository {

    private final String readQuery = "SELECT * FROM \"Friend\" WHERE username = ?";
    private final String createRequestQuery = "INSERT INTO \"Friend\" (username, friend_name, status) VALUES (?, ?, ?)";
    private final String checkRequestQuery = "SELECT * FROM \"Friend\" WHERE (username = ? AND friend_name = ? AND status = 'pending') OR (username = ? AND friend_name = ? AND status = 'pending')";
    private static final UserFriendListRepository userFriendListRepository;

    static {
        try {
            userFriendListRepository = new UserFriendListRepository();
        } catch (SQLException e) {
            throw new RuntimeException(e);
        }
    }

    public static UserFriendListRepository getInstance() {
        return userFriendListRepository;
    }

    private UserFriendListRepository() throws SQLException {
    }

    public List<String> getFriendList(String username) throws SQLException {
        PreparedStatement readStatement = connection.prepareStatement(readQuery);
        readStatement.setString(1, username);
        ResultSet resultSet = readStatement.executeQuery();
        List<String> friendList = new ArrayList<>();
        while (resultSet.next()) {
            friendList.add(resultSet.getString("friend_name"));
        }
        readStatement.close();
        return friendList;
    }

    public List<Map<String, String>> getFriendListWithStatus(String username) throws SQLException {
        String query = "SELECT friend_name, u.status AS user_status " +
                "FROM \"Friend\" f " +
                "JOIN \"User\" u ON f.friend_name = u.username " +
                "WHERE f.username = ? AND f.status = 'accepted'";
        PreparedStatement statement = connection.prepareStatement(query);
        statement.setString(1, username);
        ResultSet resultSet = statement.executeQuery();
        List<Map<String, String>> friendList = new ArrayList<>();

        while (resultSet.next()) {
            Map<String, String> friendData = new HashMap<>();
            friendData.put("friendName", resultSet.getString("friend_name"));
            friendData.put("status", resultSet.getString("user_status")); // '1' for online, '0' for offline
            friendList.add(friendData);
        }
        statement.close();
        return friendList;
    }


    public void addFriendRequest(String username, String friendName, String status) throws SQLException {
        PreparedStatement createStatement = connection.prepareStatement(createRequestQuery);
        // Insert the friend request for both directions
        try {
            // Direction 1: username -> friendName
            createStatement.setString(1, username);
            createStatement.setString(2, friendName);
            createStatement.setString(3, status);
            createStatement.executeUpdate();

            // Direction 2: friendName -> username
            createStatement.setString(1, friendName);
            createStatement.setString(2, username);
            createStatement.setString(3, status);
            createStatement.executeUpdate();
        } finally {
            createStatement.close();
        }
    }

    public boolean isFriendRequestPending(String username, String friendName) throws SQLException {
        PreparedStatement checkStatement = connection.prepareStatement(checkRequestQuery);
        checkStatement.setString(1, username);
        checkStatement.setString(2, friendName);
        checkStatement.setString(3, friendName);
        checkStatement.setString(4, username);
        ResultSet resultSet = checkStatement.executeQuery();
        boolean isPending = resultSet.next(); // Returns true if a record is found
        checkStatement.close();
        return isPending;
    }

    public void acceptFriendRequest(String username, String friendName) throws SQLException {
        String updateStatusQuery = "UPDATE \"Friend\" SET status = 'accepted' " +
                "WHERE (username = ? AND friend_name = ?) OR (username = ? AND friend_name = ?)";

        try (PreparedStatement updateStatement = connection.prepareStatement(updateStatusQuery)) {
            // Update status for both directions
            updateStatement.setString(1, username);
            updateStatement.setString(2, friendName);
            updateStatement.setString(3, friendName);
            updateStatement.setString(4, username);

            updateStatement.executeUpdate();
        }
    }

    public void deleteFriendRequest(String username, String friendName) throws SQLException {
        String deleteRequestQuery = "DELETE FROM \"Friend\" WHERE (username = ? AND friend_name = ?) OR (username = ? AND friend_name = ?)";

        try (PreparedStatement deleteStatement = connection.prepareStatement(deleteRequestQuery)) {
            // Delete the friend request for both directions
            deleteStatement.setString(1, username);
            deleteStatement.setString(2, friendName);
            deleteStatement.setString(3, friendName);
            deleteStatement.setString(4, username);

            deleteStatement.executeUpdate();
        }
    }
}

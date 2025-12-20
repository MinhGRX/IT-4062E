package server.repositories.UserInfo;


import java.sql.*;
import java.util.HashMap;

import static server.database.database.connection;


public class UserInfoRepository {
    private final String readQuery = "SELECT * FROM \"User\"";
    private final String createQuery = "INSERT INTO \"User\" (username, password) VALUES (?, ?)";
    private static final String updateStatus = "UPDATE \"User\" SET status = ? WHERE username = ?";
    private static final UserInfoRepository userInfoRepository;


    static {
        try {
            userInfoRepository = new UserInfoRepository();
        } catch (SQLException e) {
            throw new RuntimeException(e);
        }
    }


    public boolean createUser(String username, String password) throws SQLException {
        PreparedStatement createStatement = connection.prepareStatement(createQuery);
        createStatement.setString(1,username);
        createStatement.setString(2,password);
        createStatement.executeUpdate();
        createStatement.close();
        return true;
    }


    public HashMap<String,String> getUserList() throws SQLException {
        PreparedStatement readStatement = connection.prepareStatement(readQuery);
        ResultSet resultSet = readStatement.executeQuery();
        HashMap<String,String> userAccounts = new HashMap<>();
        while (resultSet.next()) {
            String username = resultSet.getString("username");
            String password = resultSet.getString("password");
            userAccounts.put(username,password);
        }
        readStatement.close();
        return userAccounts;
    }

    public String getUserStatus(String username) throws SQLException {
        String query = "SELECT status FROM \"User\" WHERE username = ?";
        PreparedStatement statement = connection.prepareStatement(query);
        statement.setString(1, username);
        ResultSet resultSet = statement.executeQuery();
        String status = null;
        if (resultSet.next()) {
            status = resultSet.getString("status");
        }
        statement.close();
        return status;
    }

    public void updateUserStatus(String username, String status) throws SQLException {
        try (PreparedStatement statement = connection.prepareStatement(updateStatus)) {
            statement.setString(1, status);  // Set the status value ('1' for online, '0' for offline)
            statement.setString(2, username);  // Set the username for which the status is being updated
            statement.executeUpdate();
        }
    }


    public static UserInfoRepository getInstance(){
        return userInfoRepository;
    }
    private UserInfoRepository() throws SQLException {
    }


}

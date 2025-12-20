package server.service;

import server.repositories.UserInfo.UserInfoRepository;

import java.sql.SQLException;

public class CheckLoginService {
    private final String type = "CheckLogin";
    private static final CheckLoginService checkLoginService = new CheckLoginService();
    private UserInfoRepository userInfoRepository = UserInfoRepository.getInstance();
    private String serverResponse;
    public String getServerResponse() {
        return serverResponse;
    }

    private CheckLoginService() {
    }
    public String response(String username, String password) throws SQLException {
         serverResponse = checkLogin(username,password);
         return  "Type: "+ type +";" + "Message: " + serverResponse;
    }
    public static CheckLoginService getInstance(){
        return checkLoginService;
    }
    private String checkLogin(String username, String password) throws SQLException {
        if (userInfoRepository.getUserList().containsKey(username)) {
            if (userInfoRepository.getUserList().get(username).equals(password)) {
                userInfoRepository.updateUserStatus(username, "1");
                return "1";
            } else
                return "0";

        }
        else return "0";
    }
}

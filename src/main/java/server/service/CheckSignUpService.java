package server.service;
import server.repositories.UserInfo.UserInfoRepository;

import java.sql.SQLException;

public class CheckSignUpService {

    private static final CheckSignUpService checkSignUpService = new CheckSignUpService();
    private final String type = "CheckSignUp";
    UserInfoRepository userInfoRepository = UserInfoRepository.getInstance();
    private CheckSignUpService(){

    }
    public static CheckSignUpService getInstance(){
        return checkSignUpService;
    }

    public String response(String username, String password) throws SQLException {
        String responseToServer = CheckSignUp(username,password);
        return "Type: "+ type +";" + "Message: " + responseToServer;
    }
    private String CheckSignUp(String username, String password) throws SQLException {
        if(!userInfoRepository.getUserList().containsKey(username)){
            userInfoRepository.createUser(username,password);
           return  "1";
        }
        return "0";
    }

}

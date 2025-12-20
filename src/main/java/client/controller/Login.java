package client.controller;

import client.service.ServerCommunicationService;
import service.CurrentTimeService;
import service.Properties;


public class Login{
    private final String header = "LOGIN";
    private static final Login login = new Login();
    private Login(){

    }

    public static Login getInstance(){
        return login;
    }
    public void sendLogin(String username, String password){
        String message =    "header: " + header +";"
                            + "currentTime: " + CurrentTimeService.getInstance().getCurrentTime() +";"
                            + "username: " + username + ";"
                            + "password: " + password;
        ServerCommunicationService.getInstance().sendMessageToServer(message);
    }
}

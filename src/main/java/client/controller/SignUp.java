package client.controller;


import client.service.ServerCommunicationService;
import service.CurrentTimeService;

public class SignUp {
    private final String header = "SIGNUP";
    private static final SignUp SignUp = new SignUp();

    private SignUp(){}

    public static SignUp getInstance(){
        return SignUp;
    }
    public void sendSignUp(String username,String password){
        String message =   "header: " + header +";"
                            + "currentTime: " + CurrentTimeService.getInstance().getCurrentTime() +";"
                            + "username: " + username + ";"
                            + "password: " + password;
        ServerCommunicationService.getInstance().sendMessageToServer(message);
    }

}

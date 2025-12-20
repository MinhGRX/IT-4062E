package client.controller;


import client.service.ServerCommunicationService;
import service.CurrentTimeService;

public class Message {
    private final String type = "SENDMESSAGE";
    private static final Message Message = new Message();
    private Message(){}

    public static Message getInstance(){
        return Message;
    }
    public void sendMessage(String message,String receiver){
        String text = "type: " + type +";"
                + "currentTime: " + CurrentTimeService.getInstance().getCurrentTime() + ";"
                + "receiver: " + receiver +";"
                + "message: " + message;
        ServerCommunicationService.getInstance().sendMessageToServer(text);
    }

}

package server.service;

import server.Handler;

import static server.Server.handlers;

public class SendMessageService {
    private final String type ="receiveMessage";
    private static final SendMessageService sendMessageService = new SendMessageService();
    private SendMessageService(){}
    public static SendMessageService getInstance(){
        return sendMessageService;
    }
    public String response(String sender, String message,String sentTime){
        return "type: "+ type + ";"
                + "sender: " + sender + ";"
                + "message: " + message + ";"
                + "sentTime: " +sentTime;
    }

    public void sendMessage(String sender,String receiver,String message,String sentTime){
        for(Handler handler: handlers){
            if (handler.getUsername().equals(receiver)){
                System.out.println(response(sender,message,sentTime));
                handler.out.println(response(sender,message,sentTime));
            }
        }
    }

}

package client.service;

import static client.Client.out;

public class ServerCommunicationService {
    private static final ServerCommunicationService SERVER_COMMUNICATION_SERVICE = new ServerCommunicationService();
    private ServerCommunicationService(){}
    public static ServerCommunicationService getInstance(){
        return SERVER_COMMUNICATION_SERVICE;
    }
    public void sendMessageToServer(String message){
        System.out.println("sent: " + message);
        out.println(message);
    }
}

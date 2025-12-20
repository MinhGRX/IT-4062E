package server.service;
import server.Handler;

import java.io.PrintWriter;

public class ClientCommunicationService {
    private final PrintWriter out;
    public ClientCommunicationService(PrintWriter out){
        this.out = out;
    };
    public  void sendMessageToServer(String message){
        System.out.println("sent: " + message);
        out.println(message);
    }
}

package server;

import server.service.*;
import service.splitService;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;


public class Handler extends Thread {
    private final Socket clientSocket;
    private String username;
    BufferedReader in;
    public PrintWriter out;
    String message;
    private Boolean isRunning = true;
    splitService splitService = service.splitService.getInstance();
    CheckLoginService checkLoginService = server.service.CheckLoginService.getInstance();
    CheckSignUpService checkSignUpService = server.service.CheckSignUpService.getInstance();
    ListFriendService listFriendService = ListFriendService.getInstance();
    LogoutService logoutService = server.service.LogoutService.getInstance();
    SendFriendRequestService sendFriendRequestService = server.service.SendFriendRequestService.getInstance();
    AcceptFriendRequestService acceptFriendRequestService = server.service.AcceptFriendRequestService.getInstance();
    SendMessageService sendMessageService = server.service.SendMessageService.getInstance();
    String currentTime;
    public  ClientCommunicationService clientCommunicationService;

    public void setCurrentTime(String currentTime) {
        this.currentTime = currentTime;
    }

    public String getCurrentTime() {
        return currentTime;
    }

    public void setRunning(Boolean running) {
        isRunning = running;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public Handler(Socket clientSocket) {
        this.clientSocket = clientSocket;
    }
    private String serverResponse;

    @Override
    public void run(){
        try {
            in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            out = new PrintWriter(clientSocket.getOutputStream(),true);
            clientCommunicationService = new ClientCommunicationService(out);
            while (true){
               message = in.readLine();
               System.out.println("received message: "+ message);
               String[] messages = message.split(";");
               for (int i = 0; i <messages.length; i++){
                   if(messages[i].startsWith("currentTime: ")){
                        setCurrentTime(splitService.splitMessage(messages[i]));
                   }
                   else {
                       messages[i] = splitService.splitMessage(messages[i]);
                   }
               }
               switch (messages[0]){
                   case "LOGIN":
                       serverResponse = CheckLoginService.getInstance().response(messages[2],messages[3]);
                       if (checkLoginService.getServerResponse().equals("1")){
                           setUsername(messages[2]);
                       }
                       break;
                   case "SIGNUP":
                       serverResponse = checkSignUpService.response(messages[2],messages[3]);
                       break;
                   case "VIEWLISTFRIEND":
                       serverResponse = listFriendService.response(username);
                       break;
                   case "SENDFRIENDREQUEST":
                       // Expecting header, currentTime, sender, and receiver in the message
                       if (messages.length >= 4) {
                           String sender = messages[2]; // Sender is at index 2
                           String receiver = messages[3]; // Receiver is at index 3

                           // Call the service and get the response
                           serverResponse = sendFriendRequestService.response(sender, receiver);
                       } else {
                           serverResponse = "Type: SENDFRIENDREQUEST; Status: 0";
                       }
                       break;
                   case "ACCEPTFRIENDREQUEST":
                       String from = messages[1];
                       String status = messages[2];
                       serverResponse = acceptFriendRequestService.response(username,from,status);
                       break;
                   case "SENDMESSAGE":
                        // Extract sender, receiver, and message content
                       String receiver = messages[2];
                       System.out.println(receiver);
                       String messageContent = messages[3];
                       System.out.println(messageContent);
                       // Process the message
                       SendMessageService.getInstance().sendMessage(username, receiver, messageContent, currentTime);

                       // Send back the server response (could be status or other response)
                       serverResponse = "Type: SENDMESSAGE; Status: 1"; // Success status
                       break;
                   case "OPENMESSAGEWINDOW":
                       //Todo: query message history, sent client
                       break;
                   case "LOGOUT":
                       serverResponse = logoutService.response(username);
                       clientSocket.close();
                       break;
               }
//               System.out.println(serverResponse);
               clientCommunicationService.sendMessageToServer(serverResponse);
            }
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }
}

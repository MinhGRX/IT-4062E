package client;


import client.view.announcement.*;
import service.splitService;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
public class Client {
    private static Client instance;
    splitService splitService = service.splitService.getInstance();
    public static BufferedReader in;
    public static PrintWriter out;
    private String loggedInUsername; // Stores the logged-in username

    String message;

    public static Client getInstance() {
        if (instance == null) {
            instance = new Client();
        }
        return instance;
    }


    public void start() throws Exception{
        Socket socket = new Socket("172.28.112.1",2403);
        in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        out = new PrintWriter(socket.getOutputStream(),true);
        while (true){
            message = in.readLine();
            System.out.println("received message: " + message);
            String[] messages = message.split(";");
            for (int i = 0; i <messages.length;i++){
                messages[i] = splitService.splitMessage(messages[i]);
            }
            boolean handled = false;
            switch (messages[0]) {
                case "CheckLogin" -> {
                    contextNotification.getInstance().setNotification(new LoginNotification());
                    handled = true;
                }
                case "CheckSignUp" -> {
                    contextNotification.getInstance().setNotification(new SignUpNotification());
                    handled = true;
                }
                case "receiveMessage" -> {
                    //ToDo: log smt
                }
                case "sendMessage" -> {
                    //ToDo: log
                }
                case "ViewListFriend" -> {
                    contextNotification.getInstance().setNotification(new friendListNotification());
                    handled = true;
                }
                case "Logout" -> {
                    contextNotification.getInstance().setNotification(new logOutNotification());
                    handled = true;
                }
                case "SendFriendRequest" -> {
                    contextNotification.getInstance().setNotification(new FriendRequestNotification());
                    handled = true;
                }

            }
            if (handled) {
                contextNotification.getInstance().executeNotification(messages[1]);
            }
        }
    }

    public String getLoggedInUsername() {
        return loggedInUsername;
    }

    public void setLoggedInUsername(String username) {
        this.loggedInUsername = username;
    }

    public static void stopClient() {
        // Here, you can stop any running threads or processes related to the client
        System.exit(0);  // This will terminate the client application
    }

    public static void main(String[] args) throws Exception {
        client.view.LoginForm.getInstance(); // login view
        Client.getInstance().start();
    }
}



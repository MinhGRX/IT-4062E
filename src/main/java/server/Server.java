package server;

import server.database.database;
import server.service.CheckOnlineService;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;


public class Server {
    private final ServerSocket serverSocket;
    public static final List<Handler> handlers = new ArrayList<>();
    private Server() throws IOException {
        serverSocket = new ServerSocket(2403);
        System.out.println("waiting for client...");
    }

//    private void checkAccountStatus(){
//        String currentTime = service.CurrentTimeService.getInstance().getCurrentTime();
//        System.out.println("daily check: "+currentTime);
//        for(Handler handler : handlers){
//            System.out.println("username:" + handler.getUsername());
//            long timeDistance = service.CurrentTimeService.getInstance().subtractTimes(currentTime,handler.getCurrentTime());
//            if(timeDistance > 10){
////                handler.setUsername("");
//                handler.clientCommunicationService.sendMessageToServer(CheckOnlineService.getInstance().response(handler.getUsername()));
//            }
//        }
//    }

    public void start() throws IOException {
//        Timer timer = new Timer();
//        TimerTask Task = new TimerTask() {
//            @Override
//            public void run() {
//                checkAccountStatus();
//            }
//        };
//        timer.schedule(Task,0,100000);
        while (true){
            Socket socket = serverSocket.accept();
            System.out.println("established: "+socket.getInetAddress().getHostAddress());
            Handler handler = new Handler(socket);
            handler.setCurrentTime(service.CurrentTimeService.getInstance().getCurrentTime());
            handlers.add(handler);
            handler.start();
        }
    }
    public static void main(String[] args) throws IOException, SQLException {
        database database = server.database.database.getInstance();
        Server server = new Server();
        server.start();
    }
}


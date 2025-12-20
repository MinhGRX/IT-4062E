package client.controller;

import client.service.ServerCommunicationService;
import service.CurrentTimeService;

public class Logout {
    private final String header = "LOGOUT";
    private static final Logout logout = new Logout();

    // Private constructor to enforce Singleton pattern
    private Logout() {
    }

    // Get the Singleton instance
    public static Logout getInstance() {
        return logout;
    }

    // Method to send a logout message to the server
    public void sendLogout() {
        String message =    "header: " + header + ";"
                + "currentTime: " + CurrentTimeService.getInstance().getCurrentTime();
        ServerCommunicationService.getInstance().sendMessageToServer(message);
    }
}

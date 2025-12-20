package client.controller;

import client.service.ServerCommunicationService;
import service.CurrentTimeService;

public class FriendRequest {
    private final String header = "SENDFRIENDREQUEST";
    private static final FriendRequest friendRequest = new FriendRequest();

    // Private constructor to enforce Singleton pattern
    private FriendRequest() {
    }

    // Get the Singleton instance
    public static FriendRequest getInstance() {
        return friendRequest;
    }

    // Method to send a friend request message to the server
    public void sendFriendRequest(String sender, String receiver) {
        String message =    "header: " + header + ";"
                + "currentTime: " + CurrentTimeService.getInstance().getCurrentTime() + ";"
                + "sender: " + sender + ";"
                + "receiver: " + receiver;
        ServerCommunicationService.getInstance().sendMessageToServer(message);
    }
}

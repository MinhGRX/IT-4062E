package client.controller;

import client.service.ServerCommunicationService;

public class AcceptFriendRequest {
    private final String header = "ACCEPTFRIENDREQUEST";
    private static final AcceptFriendRequest acceptFriendRequest = new AcceptFriendRequest();

    // Private constructor to enforce Singleton pattern
    private AcceptFriendRequest() {
    }

    // Get the Singleton instance
    public static AcceptFriendRequest getInstance() {
        return acceptFriendRequest;
    }

    // Method to send an accept/deny friend request message to the server
    public void sendResponse(String sender, String status) {
        String message = "header: " + header + ";"
                + "to: " + sender + ";"
                + "status: " + status;
        ServerCommunicationService.getInstance().sendMessageToServer(message);
    }
}

package client.controller;

import client.service.ServerCommunicationService;
import service.CurrentTimeService;

public class ViewListFriend{
    private static final ViewListFriend viewListFriend = new ViewListFriend();
    private final String type = "VIEWLISTFRIEND";

    private ViewListFriend(){};
    public static ViewListFriend getInstance(){
        return viewListFriend;
    }

    public void sendViewListFriend(){
        String message = "type: " + type + ";" + "currentTime: "+ CurrentTimeService.getInstance().getCurrentTime();
        ServerCommunicationService.getInstance().sendMessageToServer(message);
    }
}

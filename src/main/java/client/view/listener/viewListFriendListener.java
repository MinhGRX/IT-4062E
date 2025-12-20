package client.view.listener;

import client.controller.ViewListFriend;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class viewListFriendListener implements ActionListener {
    @Override
    public void actionPerformed(ActionEvent e) {
        ViewListFriend.getInstance().sendViewListFriend();
    }
}

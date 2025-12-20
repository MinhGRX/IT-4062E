package client.view.listener;

import client.view.FriendRequestForm;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class showFriendRequestFormListener implements ActionListener {
    @Override
    public void actionPerformed(ActionEvent e) {
        // Show the FriendRequestForm
        FriendRequestForm.getInstance().setVisible(true);
    }
}

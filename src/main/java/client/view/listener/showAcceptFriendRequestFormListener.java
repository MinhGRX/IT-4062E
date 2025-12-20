package client.view.listener;

import client.view.AcceptFriendRequestForm;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class showAcceptFriendRequestFormListener implements ActionListener {
    @Override
    public void actionPerformed(ActionEvent e) {
        AcceptFriendRequestForm acceptFriendRequestForm = AcceptFriendRequestForm.getInstance();
        acceptFriendRequestForm.setVisible(true);
    }
}

package client.view.listener;

import client.controller.FriendRequest;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class FriendRequestValidationListener implements ActionListener {
    private final JTextField receiverField;

    public FriendRequestValidationListener(JTextField receiverField) {
        this.receiverField = receiverField;
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        String sender = client.Client.getInstance().getLoggedInUsername();
        String receiver = receiverField.getText();

        if (receiver.isEmpty()) {
            JOptionPane.showMessageDialog(null, "Receiver cannot be empty!");
        } else {
            FriendRequest.getInstance().sendFriendRequest(sender, receiver);
            JOptionPane.showMessageDialog(null, "Friend request sent to " + receiver + "!");
        }
    }
}

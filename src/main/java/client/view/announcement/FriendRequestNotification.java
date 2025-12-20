package client.view.announcement;

import javax.swing.*;

public class FriendRequestNotification implements Notification {

    @Override
    public void notify(String message) {
        if (message.equals("1")) {
            JOptionPane.showMessageDialog(null, "Friend request sent successfully!");
        } else if (message.equals("0")) {
            JOptionPane.showMessageDialog(null, "Friend request failed. Please try again.");
        } else {
            JOptionPane.showMessageDialog(null, "Unknown response from server: " + message);
        }
    }
}

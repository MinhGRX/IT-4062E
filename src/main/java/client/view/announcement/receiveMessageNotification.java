package client.view.announcement;

import javax.swing.*;

public class receiveMessageNotification implements Notification {
    @Override
    public void notify(String message) {

        JOptionPane.showMessageDialog(null, "you have a message from " );
    }
}

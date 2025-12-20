package client.view.announcement;

import client.view.HomePage;
import client.view.LoginForm;

import javax.swing.*;

public class logOutNotification implements Notification{
    @Override
    public void notify(String message) {
        JOptionPane.showMessageDialog(HomePage.getInstance(),message);
        HomePage.getInstance().setVisible(false);
        LoginForm.getInstance().setVisible(true);
    }
}

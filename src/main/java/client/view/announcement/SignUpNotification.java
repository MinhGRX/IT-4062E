package client.view.announcement;

import client.view.SignUpForm;

import javax.swing.*;

public class SignUpNotification implements Notification{

    @Override
    public void notify(String message) {
        if(message.equals("1")) {
            JOptionPane.showMessageDialog(SignUpForm.getInstance(), "Sign Up successfully");
            client.view.SignUpForm.getInstance().setVisible(false);
            client.view.LoginForm.getInstance().setVisible(true);
        }
        else JOptionPane.showMessageDialog(SignUpForm.getInstance(), "username exists");
    }
}

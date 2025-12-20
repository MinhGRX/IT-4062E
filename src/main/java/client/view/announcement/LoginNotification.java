package client.view.announcement;

import client.view.LoginForm;
import client.Client;
import javax.swing.*;

public class LoginNotification implements Notification{

    @Override
    public void notify(String message) {
        if(message.equals("1")){
            JOptionPane.showMessageDialog(LoginForm.getInstance(),"login successfully");
            // Set the logged-in username in the Client
            String username = LoginForm.getInstance().usernameField.getText();
            Client.getInstance().setLoggedInUsername(username);

            client.view.LoginForm.getInstance().setVisible(false);
            client.view.HomePage.getInstance().setVisible(true);
        }
        else JOptionPane.showMessageDialog(LoginForm.getInstance(),"wrong username or password");
    }
}

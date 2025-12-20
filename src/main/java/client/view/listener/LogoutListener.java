package client.view.listener;

import client.Client;
import client.controller.Logout;
import client.view.HomePage;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class LogoutListener implements ActionListener {
    @Override
    public void actionPerformed(ActionEvent e) {
        Logout.getInstance().sendLogout();
        // Close the HomePage
        HomePage.getInstance().dispose();  // Dispose of the HomePage window
        // Stop the Client instance (terminate the application)
        Client.stopClient();  // Method to stop the client
    }
}

package client.view;

import client.view.listener.LogoutListener;
import client.view.listener.showFriendRequestFormListener;
import client.view.listener.viewListFriendListener;
import client.view.listener.showAcceptFriendRequestFormListener;

import javax.swing.*;
import java.awt.*;

public class HomePage extends JFrame {
    private static HomePage instance;

    private HomePage() {
        // Set up the user interface
        setTitle("Home Page");
        setSize(400, 300); // Adjusted size to fit the new button
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLocationRelativeTo(null);

        // Create buttons
        JButton viewFriendListButton = new JButton("View Friend List");
        JButton sendFriendRequestButton = new JButton("Send Friend Request");
        JButton acceptFriendRequestButton = new JButton("Accept Friend Request"); // New button
        JButton sendMessageButton = new JButton("Send Message");
        JButton logoutButton = new JButton("Logout");

        // Set up layout
        setLayout(new GridLayout(5, 1, 10, 10)); // Adjusted for 5 rows
        add(viewFriendListButton);
        add(sendFriendRequestButton);
        add(acceptFriendRequestButton); // Added the new button
        add(sendMessageButton);
        add(logoutButton);

        // Add listeners to buttons
        viewFriendListButton.addActionListener(new viewListFriendListener());
        sendFriendRequestButton.addActionListener(new showFriendRequestFormListener());
        acceptFriendRequestButton.addActionListener(new showAcceptFriendRequestFormListener()); // New listener
        sendMessageButton.addActionListener(e -> new RecipientForm().setVisible(true)); // Open RecipientForm

        // Add listener for the logout button
        logoutButton.addActionListener(new LogoutListener());
    }

    public static HomePage getInstance() {
        if (instance == null) {
            instance = new HomePage();
        }
        return instance;
    }
}

package client.view;

import client.controller.AcceptFriendRequest;

import javax.swing.*;
import java.awt.*;

public class AcceptFriendRequestForm extends JFrame {
    private static AcceptFriendRequestForm instance;

    private AcceptFriendRequestForm() {
        // Set up the form
        setTitle("Accept Friend Request");
        setSize(400, 200);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setLocationRelativeTo(null);
        setLayout(new GridLayout(3, 1, 10, 10));

        // Create components
        JPanel inputPanel = new JPanel(new FlowLayout());
        JLabel usernameLabel = new JLabel("Username: ");
        JTextField usernameField = new JTextField(20);

        JButton acceptButton = new JButton("Accept");
        JButton denyButton = new JButton("Deny");

        // Add components
        inputPanel.add(usernameLabel);
        inputPanel.add(usernameField);
        add(inputPanel);

        JPanel buttonPanel = new JPanel(new FlowLayout());
        buttonPanel.add(acceptButton);
        buttonPanel.add(denyButton);
        add(buttonPanel);

        // Add button listeners
        acceptButton.addActionListener(e -> {
            String username = usernameField.getText().trim();
            if (!username.isEmpty()) {
                AcceptFriendRequest.getInstance().sendResponse(username, "1"); // Status 1 for accept
                JOptionPane.showMessageDialog(this, "Friend request accepted.");
                dispose();
            } else {
                JOptionPane.showMessageDialog(this, "Please enter a username.");
            }
        });

        denyButton.addActionListener(e -> {
            String username = usernameField.getText().trim();
            if (!username.isEmpty()) {
                AcceptFriendRequest.getInstance().sendResponse(username, "0"); // Status 0 for deny
                JOptionPane.showMessageDialog(this, "Friend request denied.");
                dispose();
            } else {
                JOptionPane.showMessageDialog(this, "Please enter a username.");
            }
        });
    }

    public static AcceptFriendRequestForm getInstance() {
        if (instance == null) {
            instance = new AcceptFriendRequestForm();
        }
        return instance;
    }
}

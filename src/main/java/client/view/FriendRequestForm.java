package client.view;

import client.Client;
import client.view.listener.FriendRequestValidationListener;

import javax.swing.*;
import java.awt.*;

public class FriendRequestForm extends JFrame {
    private JTextField receiverField; // Made private since senderField is now auto-filled
    private static FriendRequestForm instance;

    private FriendRequestForm() {
        setTitle("Send Friend Request");
        setSize(300, 200);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setLocationRelativeTo(null);

        // Labels and fields
        JLabel senderLabel = new JLabel("Sender:");
        JLabel receiverLabel = new JLabel("Receiver:");
        JTextField senderField = new JTextField(20);
        senderField.setEditable(false); // Make senderField read-only
        senderField.setText(Client.getInstance().getLoggedInUsername()); // Set logged-in username
        receiverField = new JTextField(20);

        JButton sendButton = new JButton("Send Request");

        // Set layout and add components
        setLayout(new GridLayout(3, 2));
        add(senderLabel);
        add(senderField);
        add(receiverLabel);
        add(receiverField);
        add(new JLabel()); // Empty space
        add(sendButton);

        // Add action listener to the send button
        sendButton.addActionListener(new FriendRequestValidationListener(receiverField));

        setVisible(true);
    }

    public static FriendRequestForm getInstance() {
        if (instance == null) instance = new FriendRequestForm();
        return instance;
    }
}

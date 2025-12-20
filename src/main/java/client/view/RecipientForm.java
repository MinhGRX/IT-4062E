package client.view;

import client.view.listener.StartChattingListener;

import javax.swing.*;
import java.awt.*;

public class RecipientForm extends JFrame {
    private JTextField recipientField;
    private JButton startChatButton;

    public RecipientForm() {
        // Set up the form
        setTitle("Recipient Form");
        setSize(300, 150);
        setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        setLocationRelativeTo(null);

        // Create components
        recipientField = new JTextField(20);
        startChatButton = new JButton("Start Chatting");

        // Set up layout
        setLayout(new FlowLayout());
        add(new JLabel("Recipient Username:"));
        add(recipientField);
        add(startChatButton);

        // Add listener for the 'Start Chatting' button
        startChatButton.addActionListener(new StartChattingListener(recipientField));

        // Make the form visible
        setVisible(true);
    }
}

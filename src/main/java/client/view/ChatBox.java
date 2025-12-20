package client.view;

import client.view.listener.sendMessageListener;

import javax.swing.*;
import java.awt.*;

public class ChatBox extends JFrame {
    private JTextArea chatArea;
    private JTextField messageField;
    private JButton sendButton;
    private String recipient;

    public ChatBox(String recipient) {
        this.recipient = recipient;

        // Set up the chat window
        setTitle("Chat with " + recipient);
        setSize(400, 400);
        setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        setLocationRelativeTo(null);

        // Create components
        chatArea = new JTextArea();
        chatArea.setEditable(false);
        JScrollPane scrollPane = new JScrollPane(chatArea);

        messageField = new JTextField(20);
        sendButton = new JButton("Send");

        // Set up layout
        JPanel bottomPanel = new JPanel();
        bottomPanel.setLayout(new FlowLayout());
        bottomPanel.add(messageField);
        bottomPanel.add(sendButton);

        setLayout(new BorderLayout());
        add(scrollPane, BorderLayout.CENTER);
        add(bottomPanel, BorderLayout.SOUTH);

        // Add listener for send button using sendMessageListener
        sendButton.addActionListener(new sendMessageListener(messageField, recipient));

        // Make the form visible
        setVisible(true);
    }
}

package client.view;

import javax.swing.*;
import java.awt.*;

public class UserChatBox extends JPanel {
    private JTextArea chatHistoryArea;
    private JTextField messageField;
    private JButton sendButton;
    private String friendName;

    public UserChatBox(String friendName) {
        this.friendName = friendName;
        setLayout(new BorderLayout());

        // Chat history area
        chatHistoryArea = new JTextArea();
        chatHistoryArea.setEditable(false); // Prevent direct editing
        JScrollPane scrollPane = new JScrollPane(chatHistoryArea);

        // Message input area
        JPanel messagePanel = new JPanel(new BorderLayout());
        messageField = new JTextField();
        sendButton = new JButton("Send");
        messagePanel.add(messageField, BorderLayout.CENTER);
        messagePanel.add(sendButton, BorderLayout.EAST);

        // Add components
        add(scrollPane, BorderLayout.CENTER);
        add(messagePanel, BorderLayout.SOUTH);

        // Attach a listener to the send button
        sendButton.addActionListener(e -> sendMessage());
    }

    public void appendMessage(String sender, String message) {
        chatHistoryArea.append(sender + ": " + message + "\n");
    }

    private void sendMessage() {
        String message = messageField.getText();
        if (!message.isEmpty()) {
            appendMessage("You", message);
            messageField.setText(""); // Clear the input field
            // Send message to the server or handle further as needed
        } else {
            JOptionPane.showMessageDialog(this, "Message cannot be empty!");
        }
    }

    public String getFriendName() {
        return friendName;
    }
    public JTextArea getChatTextArea() {
        chatHistoryArea.setText(chatHistoryArea.getText());
        return null;
    }
}
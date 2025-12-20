package client.view.listener;

import client.controller.Message;
import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class sendMessageListener implements ActionListener {
    private JTextField messageField;
    private String recipient;

    public sendMessageListener(JTextField messageField, String recipient) {
        this.messageField = messageField;
        this.recipient = recipient;
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        String message = messageField.getText();
        if (!message.isEmpty()) {
            // Send the message using the Message controller
            Message.getInstance().sendMessage(message, recipient);

            // Append the message to the chat (chat history)
            // Here we assume that the chat history is displayed in a JTextArea.
//            JTextArea chatArea = ((JTextArea) SwingUtilities.getAncestorOfClass(JTextArea.class, messageField));
//            chatArea.append("You: " + message + "\n");

            // Clear the message field after sending the message
            messageField.setText("");
        } else {
            JOptionPane.showMessageDialog(null, "Message cannot be empty!");
        }
    }
}

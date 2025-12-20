package client.view.listener;

import client.view.ChatBox;
import client.controller.Message;
import client.service.ServerCommunicationService;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.*;

public class StartChattingListener implements ActionListener {
    private JTextField recipientField;

    public StartChattingListener(JTextField recipientField) {
        this.recipientField = recipientField;
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        String recipient = recipientField.getText();
        if (!recipient.isEmpty()) {
            // Fetch message history from the server or local storage
            // You may want to fetch this data from the server or database here.

            // Open the ChatBox
            new ChatBox(recipient);

            // Close the RecipientForm
            ((JFrame) SwingUtilities.getWindowAncestor(recipientField)).dispose();
        } else {
            JOptionPane.showMessageDialog(null, "Recipient username is required!");
        }
    }
}

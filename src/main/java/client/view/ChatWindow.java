package client.view;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class ChatWindow extends JFrame implements ActionListener {
    private JTextArea chatArea;
    private JTextField recipientField;
    private JButton okButton;

    public ChatWindow() {
        setTitle("Chat Window");
        setSize(400, 300);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        chatArea = new JTextArea();
        chatArea.setEditable(false);

        JScrollPane scrollPane = new JScrollPane(chatArea);

        JLabel recipientLabel = new JLabel("Recipient:");
        recipientField = new JTextField(20);
        okButton = new JButton("OK");
        okButton.addActionListener(this);

        JPanel inputPanel = new JPanel();
        inputPanel.setLayout(new FlowLayout());
        inputPanel.add(recipientLabel);
        inputPanel.add(recipientField);
        inputPanel.add(okButton);

        getContentPane().setLayout(new BorderLayout());
        getContentPane().add(scrollPane, BorderLayout.CENTER);
        getContentPane().add(inputPanel, BorderLayout.NORTH);
    }

    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == okButton) {
            String recipient = recipientField.getText();
            // Do something with the recipient
            recipientField.setText("");
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                ChatWindow chatWindow = new ChatWindow();
                chatWindow.setVisible(true);
            }
        });
    }
}
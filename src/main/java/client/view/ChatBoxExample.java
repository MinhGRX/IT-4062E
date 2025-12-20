package client.view;

import javax.swing.*;
import java.awt.*;

public class ChatBoxExample extends JFrame {

    private JTextArea chatArea;
    private JTextField inputField;

    public ChatBoxExample() {
        setTitle("Chat Box Example");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(400, 400);

        // Tạo chat area
        chatArea = new JTextArea();
        chatArea.setEditable(false);
        JScrollPane scrollPane = new JScrollPane(chatArea);
        scrollPane.setPreferredSize(new Dimension(380, 300));

        // Tạo input field
        inputField = new JTextField();
        inputField.setPreferredSize(new Dimension(300, 30));
        inputField.addActionListener(e -> sendMessage());

        // Tạo button gửi tin nhắn
        JButton sendButton = new JButton("Send");
        sendButton.addActionListener(e -> sendMessage());

        // Tạo panel chứa input field và button
        JPanel inputPanel = new JPanel();
        inputPanel.setLayout(new FlowLayout());
        inputPanel.add(inputField);
        inputPanel.add(sendButton);

        // Thêm các component vào frame
        setLayout(new BorderLayout());
        add(scrollPane, BorderLayout.CENTER);
        add(inputPanel, BorderLayout.SOUTH);
    }

    private void sendMessage() {
        String message = inputField.getText();
        chatArea.append("You: " + message + "\n");
        inputField.setText("");
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            ChatBoxExample chatBox = new ChatBoxExample();
            chatBox.setVisible(true);
        });
    }
}

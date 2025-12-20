package client.view;

import client.view.listener.UpdateFriendListListener;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class FriendListForm extends JFrame {

    public  DefaultListModel<String> friendListModel;
    private JList<String> friendList;
    public  JTextField friendNameField;
    private static final FriendListForm instance = new FriendListForm();

    public static FriendListForm getInstance(){
        return instance;
    }

    private FriendListForm() {
        // Thiết lập các thuộc tính của JFrame
        setTitle("Friend List");
        setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        setResizable(false);
        setSize(300, 400);
        setLocationRelativeTo(HomePage.getInstance());

        // Tạo một DefaultListModel để lưu danh sách bạn bè
        friendListModel = new DefaultListModel<>();

        // Tạo JList để hiển thị danh sách bạn bè
        friendList = new JList<>(friendListModel);
        JScrollPane scrollPane = new JScrollPane(friendList);
        scrollPane.setPreferredSize(new Dimension(280, 300));
        add(scrollPane, BorderLayout.NORTH);

        // Tạo JTextField để nhập tên bạn bè mới
        friendNameField = new JTextField();
        friendNameField.setPreferredSize(new Dimension(200, 30));
        add(friendNameField, BorderLayout.CENTER);

        // Tạo JButton để thêm bạn bè mới
        JButton addButton = new JButton("Add friend");
        addButton.setPreferredSize(new Dimension(80, 30));
        addButton.addActionListener(new UpdateFriendListListener());
        add(addButton, BorderLayout.SOUTH);
    }



    public static void main(String[] args) {
                getInstance().setVisible(true);
                // Gọi phương thức updateFriendList với message cần hiển thị

            }
}
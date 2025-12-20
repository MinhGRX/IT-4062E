package client.view;

import client.view.listener.LoginValidationListener;
import client.view.listener.showSignUpFormListener;

import javax.swing.*;
import java.awt.*;

public class LoginForm extends JFrame {
    public  JTextField usernameField;
    public  JPasswordField passwordField;
    private static LoginForm instance;
    private LoginForm() {
        // Thiết lập giao diện
        setTitle("Login");
        setSize(300, 200);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLocationRelativeTo(null);

        // Tạo các components
        JLabel usernameLabel = new JLabel("username:");
        JLabel passwordLabel = new JLabel("password:");
        usernameField = new JTextField(20);
        passwordField = new JPasswordField(20);
        JButton loginButton = new JButton("login");
        JButton signUpButton = new JButton("sign up");

        // Thiết lập layout
        setLayout(new GridLayout(4, 2));
        add(usernameLabel);
        add(usernameField);
        add(passwordLabel);
        add(passwordField);
        add(new JLabel()); // Ô rỗng để căn chỉnh
        add(loginButton);
        add(new JLabel()); // Ô rỗng để căn chỉnh
        add(signUpButton);
        loginButton.addActionListener(new LoginValidationListener());
        signUpButton.addActionListener(new showSignUpFormListener());
        setVisible(true);
    }

    public static LoginForm getInstance(){
        if (instance == null) instance = new LoginForm();
        return instance;
    }
}
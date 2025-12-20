package client.view;
import client.view.listener.showLoginFormListener;
import client.view.listener.signUpValidationListener;
import javax.swing.*;
import java.awt.*;
public class SignUpForm extends JFrame{
    public static JTextField usernameField;
    public static JPasswordField passwordField;
    public static JPasswordField repeatPasswordField;
    private static SignUpForm instance;
    public void announce(String message){

    }
    private SignUpForm() {
        // Set up the user interface
        setTitle("Sign Up Form");
        setSize(400, 200);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLocationRelativeTo(null);

        // Create components
        JLabel usernameLabel = new JLabel("Username:");
        usernameField = new JTextField(20);
        JLabel passwordLabel = new JLabel("Password:");
        passwordField = new JPasswordField(20);
        JLabel repeatPasswordLabel = new JLabel("Repeat Password:");
        repeatPasswordField = new JPasswordField(20);
        JButton signUpButton = new JButton("Sign Up");
        JButton loginButton = new JButton("login");
        // Set layout
        setLayout(new GridLayout(5, 2));
        add(usernameLabel);
        add(usernameField);
        add(passwordLabel);
        add(passwordField);
        add(repeatPasswordLabel);
        add(repeatPasswordField);
        add(new JLabel());
        add(signUpButton);
        add(new JLabel());
        add(loginButton);

        // Event handling for the signup button
        signUpButton.addActionListener(new signUpValidationListener());

        loginButton.addActionListener(new showLoginFormListener());
    }
    public static SignUpForm getInstance(){
        if (instance == null) instance = new SignUpForm();
        return instance;
    }
}

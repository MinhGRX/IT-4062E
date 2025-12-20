package client.view.listener;

import client.controller.Login;
import client.view.LoginForm;
import client.view.service.LoginService.LoginValidator;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;


public class LoginValidationListener implements ActionListener {

    @Override
    public void actionPerformed(ActionEvent e) {
        String username = LoginForm.getInstance().usernameField.getText();
        String password = new String(LoginForm.getInstance().passwordField.getPassword());
        int isValid = LoginValidator.getInstance().isValid(username,password);
        if( isValid ==0) {
            JOptionPane.showMessageDialog(LoginForm.getInstance(), "username or password is empty!");
            }
        else {
            Login.getInstance().sendLogin(username,password);
        }
    }
}

package client.view.listener;

import client.controller.SignUp;
import client.view.SignUpForm;
import client.view.service.SignUpService.signUpValidationService;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class signUpValidationListener implements ActionListener {
    @Override
    public void actionPerformed(ActionEvent e) {
        String username = SignUpForm.usernameField.getText();
        String password = new String(SignUpForm.passwordField.getPassword());
        String repeatPassword = new String(SignUpForm.repeatPasswordField.getPassword());
        int isValid = signUpValidationService.getInstance().isValid(username,password,repeatPassword);
        if (isValid == 0){
            JOptionPane.showMessageDialog(SignUpForm.getInstance(),"username or password or repeatPassword is empty!");
        } else if (isValid == -1) {
            JOptionPane.showMessageDialog(SignUpForm.getInstance(), "password and repeatPassword is not equal!");

        }
        else {
            SignUp.getInstance().sendSignUp(username,password);
        }
    }
}

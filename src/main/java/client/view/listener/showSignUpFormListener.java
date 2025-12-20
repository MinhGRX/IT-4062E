package client.view.listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class showSignUpFormListener implements ActionListener {
    @Override
    public void actionPerformed(ActionEvent e) {
        client.view.SignUpForm.getInstance().setVisible(true);
        client.view.LoginForm.getInstance().setVisible(false);
    }

}

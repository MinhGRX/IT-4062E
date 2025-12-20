package client.view.listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class showLoginFormListener implements ActionListener {
    @Override
    public void actionPerformed(ActionEvent e) {
        client.view.LoginForm.getInstance().setVisible(true);
        client.view.SignUpForm.getInstance().setVisible(false);
    }
}

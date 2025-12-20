package client.view.listener;

import client.view.FriendListForm;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;



public class UpdateFriendListListener implements ActionListener {
    @Override
    public void actionPerformed(ActionEvent e) {
        String friendName = FriendListForm.getInstance().friendNameField.getText();
        if (!friendName.isEmpty()) {
            FriendListForm.getInstance().friendListModel.addElement(friendName);
            FriendListForm.getInstance().friendNameField.setText("");
        }
    }
}

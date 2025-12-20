package client.view.listener;

import client.view.HomePage;
import client.view.UserChatBox;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

public class boxChatSelectionListener extends MouseAdapter {
    @Override
    public void mouseClicked(MouseEvent e) {
//       JList<String> friendList = HomePage.friendList;
//        String selectedFriend = friendList.getSelectedValue();
//
//        if (selectedFriend != null) {
//            JTabbedPane chatTabbedPane = HomePage.chatTabbedPane;
//
//            // Check if the chat tab for this friend already exists
//            for (int i = 0; i < chatTabbedPane.getTabCount(); i++) {
//                Component tab = chatTabbedPane.getComponentAt(i);
//                if (tab instanceof UserChatBox) {
//                    UserChatBox chatBox = (UserChatBox) tab;
//                    if (chatBox.getFriendName().equals(selectedFriend)) {
//                        chatTabbedPane.setSelectedIndex(i); // Focus the tab
//                        return;
//                    }
//                }
//            }
//
//            // Create a new chat tab
//            UserChatBox newChatBox = new UserChatBox(selectedFriend);
//            chatTabbedPane.addTab(selectedFriend, newChatBox);
//            chatTabbedPane.setSelectedComponent(newChatBox);
//        }
    }
}

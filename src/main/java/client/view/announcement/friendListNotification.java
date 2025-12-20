package client.view.announcement;

import client.view.FriendListForm;

public class friendListNotification implements Notification{

    @Override
    public void notify(String message) {
        FriendListForm.getInstance().setVisible(true);
        updateFriendList(message);
    }

    public void updateFriendList(String message) {
        // Xóa danh sách bạn bè hiện tại
        FriendListForm.getInstance().friendListModel.clear();

        // Chia tách chuỗi message thành các tên bạn bè
        String[] friendNames = message.split(",");
        for (String friendName : friendNames) {
            FriendListForm.getInstance().friendListModel.addElement(friendName);
        }
    }
}

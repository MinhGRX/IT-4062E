package client.view.announcement;

public class contextNotification {
    private static final contextNotification CONTEXT_NOTIFICATION = new contextNotification();
    private Notification notification;
    private contextNotification(){}
    public static contextNotification getInstance(){
        return CONTEXT_NOTIFICATION;
    }

    public void setNotification(Notification notification) {
        this.notification = notification;
    }

    public void executeNotification(String message){
        notification.notify(message);
    }
}

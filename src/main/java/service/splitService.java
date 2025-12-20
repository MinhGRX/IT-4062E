package service;

public class splitService {
    private static final splitService splitService = new splitService();

    private splitService() {
    }

    public static splitService getInstance(){
        return splitService;
    }

    public String splitMessage(String message){
        String messages[] = message.split(": ");
        return messages[1];
    }
}

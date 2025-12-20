package client.service;

public class PasswordService {
    private static final PasswordService passwordService = new PasswordService();
    private PasswordService(){};
    public static PasswordService getInstance(){
        return passwordService;
    }
    public boolean isPdEqualToRepeatPd(String password, String repeatPassword){
        if(password.equals(repeatPassword))
            return true;
        else return false;
    }


}

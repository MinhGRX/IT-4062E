package client.view.service.LoginService;

public class LoginValidator {
    private static final LoginValidator LOGIN_VALIDATOR = new LoginValidator();
    private LoginValidator(){}
    public static LoginValidator getInstance(){
        return LOGIN_VALIDATOR;
    }

    public int isValid(String username, String password){
        return selectLoginValidation(new LoginValidateService(),username,password);
    }
    private int selectLoginValidation(LoginValidation loginValidation,String username,String password){
        return loginValidation.isValid(username,password);
    }
}

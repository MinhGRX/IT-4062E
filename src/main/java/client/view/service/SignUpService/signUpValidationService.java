package client.view.service.SignUpService;

public class signUpValidationService {
    private static final signUpValidationService SIGN_UP_VALIDATION_SERVICE = new signUpValidationService();
    private signUpValidationService(){}
    public static signUpValidationService getInstance(){
        return SIGN_UP_VALIDATION_SERVICE;
    }
    public int isValid(String username, String password, String repeatPassword) {
        if (username.equals("")|| password.equals("") || repeatPassword.equals("")){
            return 0;
        } else{
            if(!password.equals(repeatPassword))
                return -1;
            return 1;
        }
    }
}

package client.view.service.LoginService;

public class LoginValidateService implements LoginValidation{

    @Override
    public int isValid(String username, String password) {
        if (username.equals("")|| password.equals("")){
            return 0;
        }
        else{
            return 1;
        }
    }
}

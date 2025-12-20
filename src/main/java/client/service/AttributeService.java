package client.service;

public class AttributeService {
    private static final AttributeService attributeService = new AttributeService();
    private AttributeService(){};
    public static AttributeService getInstance(){
        return attributeService;
    }
    public boolean isFormEmpty(String attribute){
        return attribute.equals("");
    }
}

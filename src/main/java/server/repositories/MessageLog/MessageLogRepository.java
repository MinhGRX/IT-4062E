package server.repositories.MessageLog;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.*;

import static server.database.database.connection;

public class MessageLogRepository {
    private static final MessageLogRepository messageLogRepository = new MessageLogRepository();
    private final String readQuery = "SELECT * FROM \"MessageLog\" WHERE sender = ? and receiver = ? ";
    private final String createQuery = "INSERT INTO \"MessageLog\" (sender, receiver, content, senttime) VALUES (?,?,?,?)";
    private Map<Integer,String> messageHistory = new TreeMap<>(Comparator.naturalOrder());
    private MessageLogRepository(){}

    public void addMessage(String sender, String receiver, String content, String senttime) throws SQLException {
        PreparedStatement createStatement = connection.prepareStatement(createQuery);
        createStatement.setString(1,sender);
        createStatement.setString(2,receiver);
        createStatement.setString(3, content);
        createStatement.setString(4,senttime);
        createStatement.executeUpdate();
        createStatement.close();
    }

    public void readMessageBySender(String sender, String receiver) throws SQLException {
        PreparedStatement readStatement = connection.prepareStatement(readQuery);
        readStatement.setString(1,sender);
        readStatement.setString(2,receiver);
        ResultSet resultSet = readStatement.executeQuery();
        while (resultSet.next()){
            messageHistory.put(resultSet.getInt("messageid"),resultSet.getString("sender") + ": " +
                    resultSet.getString("content") + "sentTime: " + resultSet.getString("senttime"));
        }
        readStatement.close();
    }

    public List<String> getMessageHistory(String sender, String receiver) throws SQLException {
        List<String> messageHistoryLog = new ArrayList<>();
        readMessageBySender(sender,receiver);
        readMessageBySender(receiver,sender);
        for(Map.Entry<Integer,String> entry : messageHistory.entrySet()){
            messageHistoryLog.add(entry.getValue());
        }
        return messageHistoryLog;
    }

    public static MessageLogRepository getInstance(){
        return messageLogRepository;
    }


}

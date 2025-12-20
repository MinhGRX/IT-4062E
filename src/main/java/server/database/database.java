package server.database;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class database {

    private static final database database;

    static {
        try {
            database = new database();
        } catch (SQLException e) {
            throw new RuntimeException(e);
        }
    }

    private final static String url = "jdbc:postgresql://localhost:5432/chat_application_2";
    private final static String username = "postgres";
    private final static String password = "fps360tk";

    public static final Connection connection;

    static {
        try {
            connection = DriverManager.getConnection(url, username, password);
        } catch (SQLException e) {
            throw new RuntimeException(e);
        }
    }



    private database() throws SQLException {
       System.out.println("connect database success");
    }

    public static final database getInstance(){
        return database;
    }



    public void closeConnection() throws SQLException {
        connection.close();
    }


}

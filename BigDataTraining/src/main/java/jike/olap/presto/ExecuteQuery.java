package jike.olap.presto;

import java.sql.*;

public class ExecuteQuery {
    public static void main(String[] args) throws ClassNotFoundException, SQLException {
        Class.forName("io.prestosql.jdbc.PrestoDriver ");
        String url = "jdbc:presto://knox.c-14461bcbbef08246.cn-shanghai.emr.aliyuncs.com:8443";
        Connection connection = DriverManager.getConnection(url);
        Statement statement = connection.createStatement();
        ResultSet rs = statement.executeQuery("select approx_distinct(user_id) from user_visits;");
        while (rs.next()) {
            System.out.println(rs.getInt(1));
        }
        rs.close();
        connection.close();
    }
}

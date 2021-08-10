package jike.hadoop.hbaseAction;

import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HConstants;
import org.apache.hadoop.hbase.client.Connection;
import org.apache.hadoop.hbase.client.ConnectionFactory;
import org.apache.hadoop.hbase.client.Admin;
import org.apache.hadoop.hdfs.tools.HDFSConcat;

import java.io.IOException;

public class StudentInfoProcessor {
    public static void main(String[] args) {
        Configuration conf = HBaseConfiguration.create();
        // conf.set(HConstants.ZOOKEEPER_QUORUM, "local")
        try {
            Connection connection = ConnectionFactory.createConnection(conf);
            Admin hAdmin = connection.getAdmin();
            HTableHandler tableHandler = new HTableHandler(connection, hAdmin, "xieyue:student");
            String[] columnFamilies = {"info", "score"};
            tableHandler.createTable(columnFamilies);
            tableHandler.insertValue("info", "student_id", "Tom","20210000000001");
            tableHandler.insertValue("info","class","Tom","2");
            tableHandler.insertValue("score","understanding","Tom", "75");
            tableHandler.insertValue("score","programming","Tom","82");
            tableHandler.getValue("Tom");
            tableHandler.dropTable();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}

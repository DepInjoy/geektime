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
    // 表名称
    private static String curTableName = "xieyue:student";
    // 列簇名
    private static String columnFamily1 = "info";
    private static String columnFamily2 = "score";
    // 列名
    private static String column1 = "student_id";
    private static String column2 = "class";
    private static String column3 = "understanding";
    private static String column4 = "programming";

    public static void main(String[] args) {
        Configuration conf = HBaseConfiguration.create();
        // 使用zookeeper定位
        conf.set(HConstants.ZOOKEEPER_QUORUM, "47.101.206.149:2181,47.101.216.12:2181,47.101.204.23:2181");
        // 设置HBase在HDFS中的目录
        conf.set(HConstants.HBASE_DIR, "hdfs://47.101.72.185:16020/hbase");
        try {
            Connection connection = ConnectionFactory.createConnection(conf);
            Admin hAdmin = connection.getAdmin();
            HTableHandler tableHandler = new HTableHandler(connection, hAdmin, curTableName);
            String[] columnFamilies = {"info", "score"};
            // 创建表
            tableHandler.createTable(columnFamilies);
            // 插入数据
            tableHandler.insertValue(columnFamily1, column1, "Tom","20210000000001");
            tableHandler.insertValue(columnFamily1, column2,"Tom","2");
            tableHandler.insertValue(columnFamily2, column3,"Tom", "75");
            tableHandler.insertValue(columnFamily2, column4,"Tom","82");

            tableHandler.insertValue(columnFamily1, column1, "Jerry","20210000000002");
            tableHandler.insertValue(columnFamily1, column2,"Jerry","1");
            tableHandler.insertValue(columnFamily2, column3,"Jerry", "85");
            tableHandler.insertValue(columnFamily2, column4,"Jerry","67");

            tableHandler.insertValue(columnFamily1, column1, "Jack","20210000000003");
            tableHandler.insertValue(columnFamily1, column2,"Jack","2");
            tableHandler.insertValue(columnFamily2, column3,"Jack", "80");
            tableHandler.insertValue(columnFamily2, column4,"Jack","80");

            tableHandler.insertValue(columnFamily1, column1, "Rose","20210000000004");
            tableHandler.insertValue(columnFamily1, column2,"Rose","2");
            tableHandler.insertValue(columnFamily2, column3,"Rose", "60");
            tableHandler.insertValue(columnFamily2, column4,"Rose","61");

            tableHandler.insertValue(columnFamily1, column1, "XY","G20200616010222");

            // 根据RowKey查询数据
            System.out.println("================= Get Data With RowKey = TOM Start ===================");
            tableHandler.getRow("Tom");
            System.out.println("================= Get Data With RowKey = TOM End ===================");
            System.out.println("================= Get Data With RowKey = XY Start ===================");
            tableHandler.getRow("XY");
            System.out.println("================= Get Data With RowKey = XY Start ===================");

            // 扫描获取所有的数据
            System.out.println("================= Get All Rows' Data Start ===================");
            tableHandler.getAllRows();
            System.out.println("================= Get All Rows' Data End ===================");

            // 删除表
            tableHandler.dropTable();

            hAdmin.close();
            connection.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}

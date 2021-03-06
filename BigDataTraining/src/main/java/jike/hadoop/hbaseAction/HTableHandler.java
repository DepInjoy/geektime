package jike.hadoop.hbaseAction;

import org.apache.hadoop.hbase.CellUtil;
import org.apache.hadoop.hbase.NamespaceDescriptor;
import org.apache.hadoop.hbase.client.*;
import org.apache.hadoop.hbase.util.Bytes;
import org.apache.hadoop.hbase.Cell;
import org.apache.hadoop.hbase.TableName;

import java.io.IOException;

public class HTableHandler {
    private String          curNamespace;
    private TableName       tableName;
    private Admin           admin;
    private Connection      conn;

    public HTableHandler(Connection conn, Admin admin, String tableName) {
        this.tableName = TableName.valueOf(tableName);
        this.admin = admin;
        this.conn = conn;
    }

    public void createTable(String[] columnFamily) throws IOException {
        if (!admin.tableExists(tableName)) {
            String[] split = tableName.toString().split(":");
            // 获取命名空间, 判断命名空间是否存在，如果不存在则创建
            if (split.length > 1) {
                try {
                    NamespaceDescriptor namespaceDesc = admin.getNamespaceDescriptor(split[0]);
                } catch (IOException e) {
                    System.out.println("Namespace not exist, now create " + split[0]);
                    NamespaceDescriptor namespaceDesc = NamespaceDescriptor.create(split[0]).build();
                    this.admin.createNamespace(namespaceDesc);
                }
            }

            // create 表描述生成器
            TableDescriptorBuilder tableDesc = TableDescriptorBuilder.newBuilder(tableName);
            // 添加列簇
            for (int i = 0; i < columnFamily.length;++i) {
                tableDesc.setColumnFamily(ColumnFamilyDescriptorBuilder.of(columnFamily[i]));
            }
            //利用构建表描述创建表
            admin.createTable(tableDesc.build());
        }
    }

    public void dropTable() throws IOException {
        if (admin.tableExists(tableName)) {
            //使表下架
            admin.disableTable(tableName);
            //删除表
            admin.deleteTable(tableName);
        }
    }

    public void insertValue(String columnFamily, String column, String rowKey,
                        String value) throws IOException {
        // 创建表连接对象
        Table table = conn.getTable(tableName);
        //创建put对象,传递rowKey
        Put put = new Put(rowKey.getBytes());
        //添加属性
        put.addColumn(columnFamily.getBytes(), column.getBytes(), value.getBytes());
        //添加数据到表
        table.put(put);
    }

    /*
        根据RowKey获取某一行数据信息

    * */
    public void getRow (String rowKey) throws IOException {
        Table table = conn.getTable(tableName);
        Get get = new Get(rowKey.getBytes());
        Result result = table.get(get);
        // 遍历数据
        Cell[] cells = result.rawCells();
        for (Cell cell : cells) {
            System.out.println("RowKey :"+ Bytes.toString(CellUtil.cloneRow(cell)));
            System.out.println("列簇    :"+ Bytes.toString(CellUtil.cloneFamily(cell)));
            System.out.println("列名    :"+ Bytes.toString(CellUtil.cloneQualifier(cell)));
            System.out.println("值      :"+ Bytes.toString(CellUtil.cloneValue(cell)));
            System.out.println("------------------");
        }
    }

    public void getAllRows() throws IOException {
        Table table = conn.getTable(tableName);
        // 创建用于扫描region的对象
        Scan scan = new Scan();
        ResultScanner resultScanner = table.getScanner(scan);
        for (Result result : resultScanner) {
            Cell[] cells = result.rawCells();
            for (Cell cell : cells) {
                System.out.println("RowKey :" + Bytes.toString(CellUtil.cloneRow(cell)));
                System.out.println("列簇    :" + Bytes.toString(CellUtil.cloneFamily(cell)));
                System.out.println("列名    :" + Bytes.toString(CellUtil.cloneQualifier(cell)));
                System.out.println("值      :" + Bytes.toString(CellUtil.cloneValue(cell)));
                System.out.println("+++++++++++++++++++++++++++++++++++++++++");
            }
        }
    }
}

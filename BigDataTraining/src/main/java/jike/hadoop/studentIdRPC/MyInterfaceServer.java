package jike.hadoop.studentIdRPC;

import jike.hadoop.studentIdRPC.Impl.MyInterfaceImpl;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.ipc.RPC;

import java.io.IOException;

public class MyInterfaceServer {
    public static String SERVER_ADDRESS = "127.0.0.1";
    public static int SERVER_PORT = 59098;
    public static void main(String[] args) {
        RPC.Builder builder = new RPC.Builder(new Configuration());
        // 设置服务器地址和端口
        builder.setBindAddress(SERVER_ADDRESS);
        builder.setPort(SERVER_PORT);
        builder.setProtocol(MyInterface.class);
        builder.setInstance(new MyInterfaceImpl());
        try {
            RPC.Server server = builder.build();
            server.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

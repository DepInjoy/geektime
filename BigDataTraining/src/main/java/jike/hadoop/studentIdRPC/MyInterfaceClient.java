package jike.hadoop.studentIdRPC;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.ipc.RPC;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.util.Scanner;

public class MyInterfaceClient {
    public static void main(String[] args){
        try {
            // 使用静态方法getProxy构造客户端代理对象
            MyInterface myInterface = RPC.getProxy(MyInterface.class,
                    MyInterface.versionID,/*保持和接口中的VersionID保持一致*/
                    new InetSocketAddress(MyInterfaceServer.SERVER_ADDRESS,
                            MyInterfaceServer.SERVER_PORT), new Configuration());
            // 获取用户的输入
            Scanner scanner = new Scanner(System.in);
            while (scanner.hasNext()) {
                String name = myInterface.findName(scanner.next());
                System.out.println("The student's name is " + name);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

package self.training.topic.rpc;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.ipc.RPC;
import org.apache.hadoop.ipc.RPC.Server;

import java.io.IOException;

public class DataFlowRPCServer {
    public static String SERVER_ADDRESS = "localhost";
    public static int SERVER_PORT = 39866;

    public static void main(String[] args) throws IOException {
        Configuration conf = new Configuration();
        Server server = new RPC.Builder(conf)
                .setProtocol(DataFlowInterface.class)
                .setInstance(new DataFlowProtocol(0, 0))
                .setBindAddress(SERVER_ADDRESS)
                .setPort(SERVER_PORT)
                .build();
        server.start();
    }
}

package self.training.topic.rpc;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.ipc.RPC;

import java.io.IOException;
import java.net.InetSocketAddress;

public class DataFlowClient {
    public static void main(String[] args) throws IOException {
        Configuration conf = new Configuration();
        DataFlowInterface dataFlow = RPC.getProxy(
                DataFlowInterface.class, DataFlowInterface.versionID,
                new InetSocketAddress(DataFlowRPCServer.SERVER_ADDRESS,
                        DataFlowRPCServer.SERVER_PORT), conf);
        dataFlow.setFlowData(10, 20);
        long totalFlow = dataFlow.getTotalFlow("test");
        System.out.println("Total Flow: " + totalFlow);
    }
}

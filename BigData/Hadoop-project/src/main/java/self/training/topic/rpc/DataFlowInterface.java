package self.training.topic.rpc;

import org.apache.hadoop.ipc.VersionedProtocol;

import java.io.IOException;

public interface DataFlowInterface extends VersionedProtocol {
    // 版本号，默认不同版本间客户端和服务器间不能通信
    public static long versionID = 1L;

    // 此接口必须有一个入参，否则执行会报错
    public long getTotalFlow(String msg);
    public void setFlowData(long upFlow, long downFlow);
}

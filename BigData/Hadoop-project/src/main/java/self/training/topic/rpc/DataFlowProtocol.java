package self.training.topic.rpc;

import org.apache.hadoop.ipc.ProtocolSignature;

import java.io.IOException;

public class DataFlowProtocol implements DataFlowInterface {
    private long upFlow;
    private long downFlow;

    public DataFlowProtocol(long upFlow, long downFlow) {
        this.upFlow = upFlow;
        this.downFlow = downFlow;
    }
    
    public long getTotalFlow(String msg) {
        return upFlow + downFlow;
    }

    public void setFlowData(long upFlow, long downFlow) {
        this.upFlow = upFlow;
        this.downFlow = downFlow;
    }

    public long getProtocolVersion(String protocol, long clientVersion) {
        return DataFlowInterface.versionID;
    }

    public ProtocolSignature getProtocolSignature(
            String protocol, long clientVersion, int clientMethodsHash) {
        return new ProtocolSignature(DataFlowInterface.versionID, null);
    }
}

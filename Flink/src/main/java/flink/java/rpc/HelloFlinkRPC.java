package flink.java.rpc;

import akka.actor.ActorSystem;
import org.apache.flink.runtime.akka.AkkaUtils;
import org.apache.flink.runtime.rpc.RpcEndpoint;
import org.apache.flink.runtime.rpc.RpcGateway;
import org.apache.flink.runtime.rpc.RpcService;
import org.apache.flink.runtime.rpc.akka.AkkaRpcService;
import org.apache.flink.runtime.rpc.akka.AkkaRpcServiceConfiguration;

import java.util.concurrent.CompletableFuture;


public class HelloFlinkRPC {
    public static void main(String[] args) throws Exception {
        /** 1. RPC Server端实现 */
        // 1.1. 启动RPC服务
        ActorSystem defaultActorSystem = AkkaUtils.createDefaultActorSystem();
        AkkaRpcService akkaRpcService = new AkkaRpcService(defaultActorSystem,
                AkkaRpcServiceConfiguration.defaultConfiguration());

        // 1.2. 创建RpcEndPoint
        HelloRpcEndPoint helloRpcEndPoint = new HelloRpcEndPoint(akkaRpcService, "hello-endpoint");
        helloRpcEndPoint.start();

        /** 2. RPC Client端实现 */
        // 2.1 通过SelfGateway来调用RPC,实现本地调用
        HelloRpcGateWay selfGateWay = helloRpcEndPoint.getSelfGateway(HelloRpcGateWay.class);
        String helloMsg = selfGateWay.hello();
        System.out.println("SelfGateway RPC Call: \n\t" + helloMsg);

        // 2.2 通过RpcEndPoint地址获得代理,实现远程调用
        HelloRpcGateWay helloRpcGateWay = akkaRpcService.connect(
                helloRpcEndPoint.getAddress(), HelloRpcGateWay.class).get();
        String helloMsg2 = helloRpcGateWay.hello();
        System.out.println("EndPoint Address RPC Call: \n\t" + helloMsg2);
    }

    public interface HelloRpcGateWay extends RpcGateway {
        String hello();
    }

    static class HelloRpcEndPoint extends RpcEndpoint implements HelloRpcGateWay {
        protected HelloRpcEndPoint(RpcService rpcService, String endpointId) {
            super(rpcService, endpointId);
        }

        @Override
        protected void onStart() throws Exception {
            System.out.println("HelloRpcEndPoint Start");
            super.onStart();
        }

        @Override
        protected CompletableFuture<Void> onStop() {
            System.out.println("HelloRpcEndPoint Stop");
            return super.onStop();
        }

        public String hello() {
            return "This is an example for Flink RPC";
        }
    }
}

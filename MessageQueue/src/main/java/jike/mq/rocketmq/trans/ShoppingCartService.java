package jike.mq.rocketmq.trans;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import org.apache.rocketmq.client.consumer.DefaultMQPushConsumer;
import org.apache.rocketmq.client.consumer.listener.ConsumeConcurrentlyContext;
import org.apache.rocketmq.client.consumer.listener.ConsumeConcurrentlyStatus;
import org.apache.rocketmq.client.consumer.listener.MessageListenerConcurrently;
import org.apache.rocketmq.client.exception.MQClientException;
import org.apache.rocketmq.common.message.MessageExt;
import java.util.List;

public class ShoppingCartService {
    public static void main(String[] arg) throws MQClientException {
        DefaultMQPushConsumer consumer = new DefaultMQPushConsumer("ShoppingCartConsumer");
        consumer.setNamesrvAddr("localhost:9876");
        consumer.subscribe("HelloOrderTopic", "*");
        consumer.registerMessageListener(new MessageListenerConcurrently() {
            public ConsumeConcurrentlyStatus consumeMessage(
                    List<MessageExt> msgs, ConsumeConcurrentlyContext consumeConcurrentlyContext) {
                try {
                    for (MessageExt msg : msgs) {
                        JSONObject object = JSON.parseObject(new String(msg.getBody()));
                        // 解析出OrderRequest
                        OrderRequest orderRequest = new OrderRequest(object.get("orderID").toString(),
                                Integer.valueOf(object.get("amount").toString()),
                                object.get("tag").toString());
                        System.out.println("Order Info with ID: " + orderRequest.getOrderID() +
                                " Amount: " + orderRequest.getAmount() +
                                " Tag: " + orderRequest.getTag());
                    }
                    return ConsumeConcurrentlyStatus.CONSUME_SUCCESS;
                } catch (Exception e) {
                    System.out.println("Meet Exception: " + e.getMessage());
                    return ConsumeConcurrentlyStatus.RECONSUME_LATER;
                }
            }
        });
        consumer.start();
    }
}

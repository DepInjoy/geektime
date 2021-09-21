package jike.mq.rocketmq.helloworld;

import org.apache.rocketmq.client.consumer.DefaultMQPushConsumer;
import org.apache.rocketmq.client.consumer.listener.ConsumeConcurrentlyContext;
import org.apache.rocketmq.client.consumer.listener.ConsumeConcurrentlyStatus;
import org.apache.rocketmq.client.consumer.listener.MessageListenerConcurrently;
import org.apache.rocketmq.client.exception.MQClientException;
import org.apache.rocketmq.common.message.MessageExt;

import java.util.List;

public class HelloCustomer {
    public static void main(String[] args) throws MQClientException {
        // 初始化消费者实例
        DefaultMQPushConsumer pushConsumer = new DefaultMQPushConsumer("HelloWorldConsumerGroup");
        // 设置NameServer的地址
        pushConsumer.setNamesrvAddr("localhost:9876");
        // 订阅Topic以及Tag来过滤需要消费的消息
        pushConsumer.subscribe("HelloTopic", "*");
        // 注册回调实现类来处理从broker拉取回来的消息
        pushConsumer.registerMessageListener(new MessageListenerConcurrently() {
            public ConsumeConcurrentlyStatus consumeMessage(
                    List<MessageExt> msgs, ConsumeConcurrentlyContext consumeConcurrentlyContext) {
                System.out.printf("%s Receive Messages %s %n" , Thread.currentThread().getName(), msgs);
                return ConsumeConcurrentlyStatus.CONSUME_SUCCESS;
            }
        });
        // 启动消费者
        pushConsumer.start();
    }
}

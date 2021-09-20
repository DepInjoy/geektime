package jike.mq.rocketmq.helloworld;

import org.apache.rocketmq.client.producer.DefaultMQProducer;
import org.apache.rocketmq.client.producer.SendCallback;
import org.apache.rocketmq.client.producer.SendResult;
import org.apache.rocketmq.client.producer.SendStatus;
import org.apache.rocketmq.common.CountDownLatch2;
import org.apache.rocketmq.common.message.Message;
import org.apache.rocketmq.remoting.common.RemotingHelper;

import java.util.concurrent.TimeUnit;

public class SyncProducer {
    // 定义发送消息的方式
    private enum SendMethod {
        SYNC,       /** 同步发送 */
        ASYNC,      /** 异步发送 */
        ONEWAY     /** 单向发送 */
    };
    // 定义发送消息的方式
    private static SendMethod sendMethod = SendMethod.ONEWAY;

    public static void main(String[] args) {
        // 初始化生产者实例
        DefaultMQProducer producer = new DefaultMQProducer("HelloWorldProducerGroup");
        // 设置NameServer的地址
        producer.setNamesrvAddr("localhost:9876");
        try {
            // 启动生产者实例
            producer.start();
            producer.setRetryTimesWhenSendAsyncFailed(0);

            // 根据消息数量实例化倒计时计算器,对于异步模式是必须的
            CountDownLatch2 countDownLatch = new CountDownLatch2(10);

            // 同步发送10条消息
            for (int i = 0; i < 10; i++) {
                final int msgIndex = i;
                Message msg = new Message("TopicTest", /* Topic */
                        "TagA", /* Tag */
                        ("Hello World RocketMQ " + i).getBytes(RemotingHelper.DEFAULT_CHARSET) /* Message Body */);
                switch (sendMethod) {
                    case SYNC:
                        // 采用同步方式发送消息到Broker
                        SendResult result = producer.send(msg);
                        if (result.getSendStatus() != SendStatus.SEND_OK) {
                            System.out.println(" Send message error: " + result);
                        } else {
                            System.out.println("Send message OK");
                        }
                        break;
                    case ASYNC:
                        // callback方式异步发送消息
                        producer.send(msg, new SendCallback() {
                            public void onSuccess(SendResult sendResult) {
                                System.out.println("send message success " + msgIndex);
                            }

                            public void onException(Throwable e) {
                                System.out.println("Exception " + msgIndex + e);
                                e.printStackTrace();
                            }
                        });
                        break;
                    case ONEWAY:
                        // 采用oneway方式发送消息，无返回结果
                        producer.sendOneway(msg);
                        break;
                }
                // 等待5秒
                countDownLatch.await(2, TimeUnit.SECONDS);
            }
            // 不再发送消息，关闭生产者
            producer.shutdown();
        } catch (Exception e) {
            System.out.println("Oh, No meet some exception for " + e.getMessage());
        }
    }
}

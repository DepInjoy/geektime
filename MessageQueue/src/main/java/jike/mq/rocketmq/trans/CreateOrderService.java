package jike.mq.rocketmq.trans;

import com.alibaba.fastjson.JSON;
import org.apache.rocketmq.client.exception.MQClientException;
import org.apache.rocketmq.client.producer.*;
import org.apache.rocketmq.common.message.Message;
import org.apache.rocketmq.remoting.common.RemotingHelper;

import java.io.UnsupportedEncodingException;
import java.util.concurrent.*;
/*
* 订单系统(生产者)
*   1、订单系统开启事务
*   2、订单系统发送半消息
*   3、执行本地事务，在订单库中创建订单
*   4、提交或回滚
*
* 购物车系统(消费者)
*   1、获取订单信息
* */

public class CreateOrderService {
    private TransactionMQProducer       producer;
    private ExecutorService             executorService;

    public void init() throws MQClientException {
        TransactionListener transListener = new OrderTransactionListener();
        producer = new TransactionMQProducer("order-system-producer-group");
        executorService = new ThreadPoolExecutor(2, 5,
                10, TimeUnit.SECONDS,
                new ArrayBlockingQueue<Runnable>(2000), new ThreadFactory() {
            public Thread newThread(Runnable r) {
                Thread thread = new Thread(r);
                thread.setName("client-transaction-msg-check-thread");
                return thread;
            }
        });
        producer.setNamesrvAddr("localhost:9876");
        producer.setExecutorService(executorService);
        producer.setTransactionListener(transListener);
        producer.start();
    }

    private Message createMessage(OrderRequest request)
            throws UnsupportedEncodingException {
        Message message = new Message("HelloOrderTopic",
                JSON.toJSONString(request).getBytes(RemotingHelper.DEFAULT_CHARSET));
        return  message;
    }

    /**
     * 创建订单
     */
    public Boolean createOrder(OrderRequest request) {
        try {
            Message msg = createMessage(request);
            // 发送事务消息
            SendResult result = producer.sendMessageInTransaction(msg, request);
            return result.getSendStatus() == SendStatus.SEND_OK;
        } catch (UnsupportedEncodingException e) {
            System.out.println("Meet UnsupportedEncodingException : " + e.getMessage());
            return false;
        } catch (MQClientException e) {
            System.out.println("Meet MQClientException : " + e.getMessage());
            return false;
        }
    }

    public void close() {
        producer.shutdown();
    }
}

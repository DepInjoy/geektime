package jike.mq.rocketmq.trans;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import org.apache.rocketmq.client.producer.LocalTransactionState;
import org.apache.rocketmq.client.producer.TransactionListener;
import org.apache.rocketmq.common.message.Message;
import org.apache.rocketmq.common.message.MessageExt;

public class OrderTransactionListener implements TransactionListener {
    private OrderDao orderDao = new OrderDao();
    /**
     * @param message
     * @param arg
     */
    public LocalTransactionState executeLocalTransaction(Message message, Object arg) {
        System.out.println("执行本地事务");
        OrderRequest request = (OrderRequest) arg;
        try {
            orderDao.createOrder(request);
            return LocalTransactionState.COMMIT_MESSAGE;
        } catch (Throwable e) {
            e.printStackTrace();
            // 创建订单失败，事务回滚
            return LocalTransactionState.ROLLBACK_MESSAGE;
        }
    }

    /**
     * @param msg
     * @return
     */
    public LocalTransactionState checkLocalTransaction(MessageExt msg) {
        System.out.println("反查本地事务状态");
        JSONObject object = JSON.parseObject(new String(msg.getBody()));
        String oderID = object.get("orderID").toString();

        // 订单存在则提交事务，否则，可能是本地事务失败或执行中，返回UNKNOWN
        return orderDao.isOrderExisted(oderID) ?
                LocalTransactionState.COMMIT_MESSAGE :
                LocalTransactionState.UNKNOW;
    }
}

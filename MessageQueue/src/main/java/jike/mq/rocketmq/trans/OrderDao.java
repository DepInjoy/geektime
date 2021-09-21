package jike.mq.rocketmq.trans;

import java.util.concurrent.ConcurrentHashMap;

public class OrderDao {
    // 模拟订单系统
    private ConcurrentHashMap<String, OrderRequest> orderMap =
            new ConcurrentHashMap<String, OrderRequest>();

    public Boolean isOrderExisted(String orderID) {
        return orderMap.containsKey(orderID);
    }

    public void createOrder(OrderRequest request) {
        orderMap.put(request.getOrderID(), request);
    }
}

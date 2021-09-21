package jike.mq.rocketmq.trans;

import org.apache.rocketmq.client.exception.MQClientException;

public class OrderServiceImpl {
    public static void main(String[] args) {
        try {
            CreateOrderService createOrderService = new CreateOrderService();
            createOrderService.init();
            for (int i = 0; i < 10;i++) {
                OrderRequest request = new OrderRequest("2021-9-23 " + i,
                        100, "Car");
                Boolean success = createOrderService.createOrder(request);
                if (success) {
                    System.out.println("Create Order Successfully !");
                }
            }
            createOrderService.close();
        } catch (MQClientException e) {
            System.out.println("Meet Exception: " + e.getMessage());
        }
    }
}

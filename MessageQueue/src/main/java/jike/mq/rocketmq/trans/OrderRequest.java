package jike.mq.rocketmq.trans;

public class OrderRequest {
    private String      orderID;    // 订单编号
    private Integer     amount;     // 订单金额
    private String      tag;

    OrderRequest(String id, Integer amount, String tag) {
        this.orderID = id;
        this.amount = amount;
        this.tag = tag;
    }

    public String getOrderID() {
        return orderID;
    }

    public Integer getAmount() {
        return amount;
    }

    public String getTag() { return tag; }
}

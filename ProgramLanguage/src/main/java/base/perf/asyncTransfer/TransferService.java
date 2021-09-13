package base.perf.asyncTransfer;

import java.util.concurrent.CompletableFuture;

public interface TransferService {
    /**
     * @function                异步转账服务
     * @param fromAccountId     转出账户ID
     * @param toAccountId       转入账户ID
     * @param amount            转账金额
     * */
    CompletableFuture<Void> transfer(int fromAccountId, int toAccountId, int amount);
}

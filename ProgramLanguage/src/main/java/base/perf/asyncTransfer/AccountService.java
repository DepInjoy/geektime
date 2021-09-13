package base.perf.asyncTransfer;

import java.util.concurrent.CompletableFuture;

public interface AccountService {
    /**
     * @function:           变更账户金额
     *
     * @param accountId:    账户ID
     * @param amount:       转账金额
    * */
    CompletableFuture<Void> add(int accountId, int amount);
}

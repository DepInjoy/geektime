package base.perf.asyncTransfer;

import java.util.concurrent.CompletableFuture;

public interface AccountService {
    /**
     * @function:           变更账户金额
     *
     * @param accountId:    账户ID
     * @param amount:       转账金额
     *
     * @return:             -1: Failed, 0:successfully
    * */
    CompletableFuture<Integer> add(int accountId, int amount) throws RuntimeException;
}

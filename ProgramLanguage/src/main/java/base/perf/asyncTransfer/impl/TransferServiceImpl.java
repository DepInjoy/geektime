package base.perf.asyncTransfer.impl;

import base.perf.asyncTransfer.AccountService;
import base.perf.asyncTransfer.TransferService;

import java.util.concurrent.CompletableFuture;

public class TransferServiceImpl  implements TransferService {
    private AccountService accountService = new AccountServiceImpl();

    @Override
    public CompletableFuture<Void> transfer(int fromAccountId, int toAccountId, int amount) {
        // 异步调用add方法将fromAccountId扣掉amount
        return accountService.add(fromAccountId, -1 * amount)
                // 调用add方法将toAccountId增加amount
                .thenCompose(v -> accountService.add(toAccountId, amount));
    }
}

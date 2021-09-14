package base.perf.asyncTransfer.impl;

import base.perf.asyncTransfer.AccountService;
import base.perf.asyncTransfer.TransferService;

import java.util.concurrent.CompletableFuture;
import java.util.concurrent.atomic.AtomicReference;

public class TransferServiceImpl  implements TransferService {
    private AccountService accountService = new AccountServiceImpl();

    private Boolean retryTransfer(Integer accountId, Integer amount) {
        AtomicReference<Boolean> success = new AtomicReference<>(false);
        Integer retryTimes = 0;
        do {
            try {
                accountService.add(accountId, amount).thenAccept((transferRes) -> {
                    if (transferRes >= 0) {
                        success.set(true);
                        System.out.println(accountId + " recover " + amount + "successfully");
                    } else success.set(false);
                }).exceptionally((e) -> {
                    e.printStackTrace();
                    return null;
                });
            } catch (RuntimeException e) {
                e.printStackTrace();
            }
            retryTimes = retryTimes + 1;
            if (success.get()) break;
        } while (retryTimes >= 5);
        System.out.println(accountId + " recover " + amount
                + " retry " + retryTimes + " failed");
        return success.get();
    }

    @Override
    public CompletableFuture<Integer> transfer(int fromAccountId, int toAccountId, int amount) {
        AtomicReference<Integer> result = new AtomicReference<>(-1);
        // 异步调用add方法将fromAccountId扣掉amount
        return accountService.add(fromAccountId, -1 * amount).thenApplyAsync((res) -> {
            /** fromAccountId账户, 如果执行成功 */
            if (res >= 0) {
                // 调用add方法将toAccountId增加amount
                accountService.add(toAccountId, amount).thenApplyAsync((fromRes) -> {
                    if (fromRes >= 0) {
                        System.out.println(fromAccountId + " and " + toAccountId
                                + " transfer " + amount + "successfully");
                        return 0;
                    } else {
                        if (retryTransfer(fromAccountId, amount)) return 0;
                        else return -1;
                    }
                }).exceptionally((toException) -> {
                    toException.printStackTrace();
                    if(retryTransfer(fromAccountId, amount)) result.set(0);
                    else result.set(-1);
                    return null;
                });
            } else {
                if(retryTransfer(fromAccountId, amount)) result.set(0);
                else result.set(-1);
            }
            return result.get();
        }).exceptionally((fromException) -> {
            /** 执行异常处理 */
            System.out.println(fromAccountId + " transfer "  + amount + " failed and "
                    + toAccountId  + " with no try" );
            fromException.printStackTrace();
            return null;
        });
    }
}

package base.perf.asyncTransfer.impl;

import base.perf.asyncTransfer.AccountService;

import java.util.HashMap;
import java.util.concurrent.CompletableFuture;

public class AccountServiceImpl implements AccountService {
    private HashMap<Integer, Integer> accountMap = new HashMap<>();
    public AccountServiceImpl() {
        accountMap.put(101, 1000);
        accountMap.put(102, 1000);
    }


    @Override
    public CompletableFuture<Integer> add(int accountId, int amount) throws RuntimeException {
        return CompletableFuture.supplyAsync(() -> {
            if (accountMap.containsKey(accountId)) {
                Integer curAmount = accountMap.get(accountId);
                if (curAmount + amount < 0) {
                    System.out.println("accountID " + accountId + " try to add "
                            + amount + " failed with " + curAmount);
                    return -1;
                }
                accountMap.put(accountId, curAmount + amount);
                System.out.println("accountId " + accountId + "  current amount " + accountMap.get(accountId));
                return 0;
            } else {
                throw new RuntimeException("accountId "+ accountId + " not existed");
            }
        });
    }
}

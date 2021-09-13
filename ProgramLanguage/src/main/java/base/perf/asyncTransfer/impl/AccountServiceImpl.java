package base.perf.asyncTransfer.impl;

import base.perf.asyncTransfer.AccountService;

import java.util.HashMap;
import java.util.concurrent.CompletableFuture;

public class AccountServiceImpl implements AccountService {
    private HashMap<Integer, Integer> accountMap = new HashMap<>();

    @Override
    public CompletableFuture<Void> add(int accountId, int amount) {
        return CompletableFuture.runAsync(() -> {
            if (accountMap.containsKey(accountId)) {
                accountMap.put(accountId, accountMap.get(accountId) + amount);
                System.out.println("accountId " + accountId + "  current amount " + accountMap.get(accountId));
            } else {
                accountMap.put(accountId, amount);
                System.out.println("accountId " + accountId + " not existed, now create it");
            }
        });
    }
}

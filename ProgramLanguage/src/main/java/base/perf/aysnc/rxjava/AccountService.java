package base.perf.aysnc.rxjava;

import java.util.HashMap;
import java.util.concurrent.CompletableFuture;

public class AccountService {
    private HashMap<Integer, Integer> accountMap = new HashMap<>();
    public AccountService() {
        accountMap.put(101, 1000);
        accountMap.put(102, 1000);
    }

    public Integer add(int accountId, int amount) throws RuntimeException {
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
    }
}

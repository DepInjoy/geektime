package base.perf.asyncTransfer;

import base.perf.asyncTransfer.impl.TransferServiceImpl;

import java.util.concurrent.ExecutionException;

public class AccountTransferService {
    private static TransferService transferService = new TransferServiceImpl();
    private static Integer accountA = 101;
    private static Integer accountB = 102;

    public static void syncDo () {
        try {
            transferService.transfer(accountA, accountB, 100).get();
            System.out.println("同步转账完成");
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (ExecutionException e) {
            e.printStackTrace();
        }
    }

    public static void asyncDo () {
        transferService.transfer(accountA, accountB, 100).thenRun(() ->{
            System.out.println("异步转账完成");
        });
    }

    public static void main(String[] args) {
        System.out.println("syncDo Start");
        syncDo();
        System.out.println("syncDo End");

        System.out.println("asyncDo Start");
        asyncDo();
        System.out.println("asyncDo End");    }
}

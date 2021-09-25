package base.perf.aysnc.rxjava;

import io.reactivex.Observable;
import io.reactivex.ObservableEmitter;
import io.reactivex.ObservableOnSubscribe;
import io.reactivex.Observer;
import io.reactivex.annotations.NonNull;
import io.reactivex.disposables.Disposable;
import javafx.util.Pair;

public class AysncTransferService {
    public static void main(String[] args) {
        AccountService accountService = new AccountService();
        Pair<Integer, Integer> accountA = new Pair<Integer, Integer>(101, 100);
        Pair<Integer, Integer> accountB = new Pair<Integer, Integer>(102, -100);

        Observable.create(new ObservableOnSubscribe< Pair<Integer, Integer> >() {
            /** 被观察者 */
            @Override
            public void subscribe(@NonNull ObservableEmitter<Pair<Integer, Integer>>
                                          observableEmitter) throws Exception {
                if (accountService.add(accountA.getKey(), accountA.getValue()) >= 0) {
                    observableEmitter.onNext(accountB);
                } else {
                    observableEmitter.onError(new Exception("Account "
                            + accountA.getKey() + " Transfer failed"));
                }
            }
        }).subscribe(new Observer< Pair<Integer, Integer> >() {
            /** 观察者 */
            @Override
            public void onSubscribe(@NonNull Disposable disposable) {
                System.out.println("onSubscribe");
            }

            @Override
            public void onNext(@NonNull Pair<Integer, Integer> info) {
                System.out.println("onNext");
                if (accountService.add(info.getKey(), info.getValue()) >= 0) {
                    System.out.println("Transfer successfully !");
                } else {
                    // 给账户B转账失败，尝试6次将A账户的钱扣除
                    Integer retry = 6;
                    do {
                        if(accountService.add(accountA.getKey(), -1 * accountA.getValue()) >= 0) {
                            System.out.println("Account " + accountA.getKey() + " rollback successfully");
                            return;
                        }
                    } while (retry >= 0);
                    System.out.println("NOTIFY Account" + accountB.getKey()
                            + " Transfer failed and " + accountA.getKey() +
                            "Transfer successfully");
                }
            }

            @Override
            public void onError(@NonNull Throwable e) {
                System.out.println("onError for : " + e.getMessage());
            }

            @Override
            public void onComplete() {
                System.out.println("onComplete");
            }
        });
    }
}

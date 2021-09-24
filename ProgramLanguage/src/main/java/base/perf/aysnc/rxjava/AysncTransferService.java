package base.perf.aysnc.rxjava;

import io.reactivex.Observable;
import io.reactivex.ObservableEmitter;
import io.reactivex.ObservableOnSubscribe;
import io.reactivex.Observer;
import io.reactivex.annotations.NonNull;
import io.reactivex.disposables.Disposable;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class AysncTransferService {
    public static void main(String[] args) {
        ConcurrentHashMap<String, Integer> accountMap = new ConcurrentHashMap<String, Integer>();
        Observable.create(new ObservableOnSubscribe< Map.Entry<String, Integer> >() {
            @Override
            public void subscribe(@NonNull ObservableEmitter< Map.Entry<String, Integer> > observableEmitter) throws Exception {

            }
        }).subscribe(new Observer< Map.Entry<String, Integer> >() {
            @Override
            public void onSubscribe(@NonNull Disposable disposable) {

            }

            @Override
            public void onNext(@NonNull Map.Entry<String, Integer> entry) {

            }

            @Override
            public void onError(@NonNull Throwable throwable) {

            }

            @Override
            public void onComplete() {

            }
        });
    }
}

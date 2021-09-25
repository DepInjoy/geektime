package base.multithread;

import com.sun.xml.internal.ws.util.CompletedFuture;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

public class CASOrFAADemo {
    private static Integer TOTAL_THREADS_COUNTER = 1000;
    private static String increaseMethod = "FAA"; // or CAS
    public static void main(String[] args) {
        // counter of thread task
        final CountDownLatch threadCountLatch = new CountDownLatch(TOTAL_THREADS_COUNTER);
        // init total as 0
        final AtomicInteger total = new AtomicInteger(0);
        try {
            // create thread pool
            ThreadPoolExecutor threadPoolExecutor = (ThreadPoolExecutor)
                    Executors.newFixedThreadPool(5);
            for (int i = 0; i < TOTAL_THREADS_COUNTER;++i) {
                threadPoolExecutor.execute(new Runnable() {
                    public void run() {
                        try {
                            if (increaseMethod == "CAS") {
                                // CAS, Increase 1
                                while (!total.compareAndSet(total.get(), total.get() + 1)) {
                                }
                                ;
                            } else if (increaseMethod == "FAA") {
                                total.getAndIncrement();
                            }
                            threadCountLatch.countDown();
                            TimeUnit.MICROSECONDS.sleep(100);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                });
            }
            // hung up and wait until 0
            threadCountLatch.await();
            System.out.println(total.get());
            System.exit(0);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

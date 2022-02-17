package algorithm.training.algorithm.reservoirSampling;

/**
 * @brief:      蓄水池算法
 *
 * */
public class D29_03_ReservoirSampling {
    public static class RandomBox {
        private int[] bag;
        // 蓄水池容量
        private int N;
        // 接收的数据量
        private int count;

        public RandomBox(int capacity) {
            bag = new int[capacity];
            N = capacity;
            count = 0;
        }

        public void add(int num) {
            count++;
            // 接收的数据量小于蓄水池容量，直接放入蓄水池
            if (count < N) {
                bag[count - 1] = num;
            } else {
                // 在[0, count]范围内取一随机数d,若d的落在[0, N-1]范围内
                // 用接收到的数据替换蓄水池中的第d个数据
                if (random(count) <= N) {
                    bag[random(N)-1] = num;
                }
            }
        }

        public int[] choices() {
            int[] ans = new int[N];
            for (int i = 0; i < N; i++) {
                ans[i] = bag[i];
            }
            return ans;
        }

        // 产生1-max的随机数
        private int random(int max) {
            return (int)(Math.random() * max) + 1;
        }
    }

    public static void main(String[] args) {
        System.out.println("hello");
        int all = 100;
        int choose = 10;
        int testTimes = 50000;
        int[] counts = new int[all + 1];
        for (int i = 0; i < testTimes; i++) {
            RandomBox box = new RandomBox(choose);
            for (int num = 1; num <= all; num++) {
                box.add(num);
            }
            int[] ans = box.choices();
            for (int j = 0; j < ans.length; j++) {
                counts[ans[j]]++;
            }
        }

        for (int i = 0; i < counts.length; i++) {
            System.out.println(i + " times : " + counts[i]);
        }
    }
}

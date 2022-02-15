package algorithm.training.struct.IndexTree;

/**
 * @brief:      支持区间查询，只支持单点更新
 *              对于一维处理弱于线段树, 但可以很容易地扩展到二维和三维
 *
 *              操作复杂度是O(logN)
 * */
public class D32_01_IndexTree {
    // 下标从1开始
    public static class IndexTree {
        private int[] tree;
        private int N;

        public IndexTree(int size) {
            N = size;
            tree = new int[N + 1];
        }

        // 计算1到index的累计和
        public int sum(int index) {
            int ret = 0;
            while (index > 0) {
                ret += tree[index];
                index -= index & (~index + 1);
            }
            return ret;
        }

        // index添加一个数字d
        public void add(int index, int delta) {
            while (index <= N) {
                tree[index] += delta;
                index += index & (~index + 1);
            }
        }
    }

    /******************** For Test *************************/
    public static class Right {
        private int[] nums;
        private int N;

        public Right(int size) {
            N = size + 1;
            nums = new int[N + 1];
        }

        public int sum(int index) {
            int ret = 0;
            for (int i = 1; i <= index; i++) {
                ret += nums[i];
            }
            return ret;
        }

        public void add(int index, int d) {
            nums[index] += d;
        }

    }

    public static void main(String[] args) {
        int N = 100;
        int V = 100;
        int testTime = 2000000;
        IndexTree tree = new IndexTree(N);
        Right test = new Right(N);
        System.out.println("test begin");
        for (int i = 0; i < testTime; i++) {
            int index = (int) (Math.random() * N) + 1;
            if (Math.random() <= 0.5) {
                int add = (int) (Math.random() * V);
                tree.add(index, add);
                test.add(index, add);
            } else {
                if (tree.sum(index) != test.sum(index)) {
                    System.out.println("Oops!");
                }
            }
        }
        System.out.println("test finish");
    }
}

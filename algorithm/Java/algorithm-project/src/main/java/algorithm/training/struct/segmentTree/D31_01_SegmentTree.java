package algorithm.training.struct.segmentTree;

/**
 * @brief:              线段树SegmentTree
 *
 *              实现功能：
 *                  1. update实现将原数组中L到R的数更新为C
 *                  2. add实现将原数组中L到R的数更新为C
 *                  3. query实现求L到R的和
 *
 *              采用懒更新的方式达到O(logN)的时间复杂度
 * */
public class D31_01_SegmentTree {
    public static class SegmentTree {
        private int  MAXN;
        // 原序列信息, 在arr中从1开始
        private int[] arr;
        // 线段树维护区间和
        private int[] sum;
        // 用于将L到R上的数字增加C的懒更新标志，其中为0表示已经完成下推
        private int[] lazy;
        // update和change结合用于将L到R更新值的懒更新标志
        // change用于记录更新值，update用于记录相关更新是否已经完成下推
        private int[] change;
        private boolean[] update;

        public SegmentTree(int[] origin) {
            MAXN = origin.length + 1;
            arr = new int[MAXN];
            for (int i = 1; i < MAXN;++i) {
                arr[i] = origin[i - 1];
            }
            // 用于某个范围的累加和信息
            sum = new int[MAXN << 2];
            // 用于支持某个范围没有继续向下传递的累加任务
            lazy = new int[MAXN << 2];
            // 用于支持某个范围有没有更新操作任务
            change = new int[MAXN << 2];
            // 懒更新标志
            update = new boolean[MAXN << 2];
        }

        // 初始化sum数组，si为l到r范围在sum数组中的索引
        public void build(int l, int r, int rt) {
            if (l == r) {
                sum[rt] = arr[l];
                return;
            }
            int mid = (l + r) >> 1;
            build(l, mid, rt << 1);
            build(mid + 1, r, rt << 1 | 1);
            pushUp(rt);
        }

        // 原始数组想要将L~R范围内的所有值更新为C
        // l和r分别表示线段树节点表示范围的左值和右值，rt表示线段树在数组表示中的索引
        public void update(int L, int R, int C, int l, int r, int rt) {
            if (L <= l && r <= R) {
                update[rt] = true;
                change[rt] = C;
                sum[rt] = C * (r - l + 1);
                lazy[rt] = 0;
                return;
            }

            int mid = (l + r) >> 1;
            pushDown(rt, mid - l + 1, r - mid);
            if (L <= mid) {
                update(L, R, C, l, mid, rt << 1);
            }
            if (mid < R) {
                update(L, R, C, mid+1, r, rt << 1 | 1);
            }
            pushUp(rt);
        }

        // 在原始数组中L~R范围内每个数都增加C
        // l和r分别表示线段树节点表示范围的左值和右值，rt表示线段树在数组表示中的索引
        public void add(int L, int R, int C, int l, int r, int rt) {
            if (L <= l && r <= R) {
                lazy[rt] += C;
                sum[rt] += C * (r - l + 1);
                return;
            }

            int mid = (l + r) >> 1;
            pushDown(rt, mid - l + 1, r - mid);
            if (L <= mid) {
                add(L, R, C, l, mid, rt << 1);
            }
            if (mid < R) {
                add(L, R, C, mid + 1, r, rt << 1 | 1);
            }
            pushUp(rt);
        }

        // 计算L~R上的累计和
        public long query(int L, int R, int l, int r, int rt) {
            if (L <= l && r <= R) {
                return sum[rt];
            }

            int mid = (r + l) >> 1;
            long ans = 0;
            pushDown(rt, mid - l + 1, r - mid);
            if (L <= mid) {
                ans += query(L, R, l, mid, rt << 1);
            }
            if (mid < R) {
                ans += query(L, R, mid + 1, r, rt << 1 | 1);
            }
            return ans;
        }

        // 用左右两个孩子的累加和来更新根节点的值
        private void pushUp(int rt) {
            sum[rt] = sum[rt << 1] + sum[rt << 1 | 1];
        }

        private void pushDown(int rt, int ln, int rn) {
            // 存在懒更新数据，将更新下推
            if (update[rt]) {
                update[rt << 1] = true;
                update[rt << 1 | 1] = true;
                change[rt << 1] = change[rt];
                change[rt << 1 | 1] = change[rt];
                lazy[rt << 1] = 0;
                lazy[rt<< 1 | 1] = 0;
                sum[rt << 1] = change[rt] * ln;
                sum[rt << 1 | 1] = change[rt] * rn;
                update[rt] = false;
            }

            // 区间增加值更新,将其下推
            if (lazy[rt] != 0) {
                lazy[rt << 1] += lazy[rt];
                lazy[rt << 1 | 1] += lazy[rt];
                sum[rt << 1] += lazy[rt] * ln;
                sum[rt << 1| 1] += lazy[rt] * rn;
                lazy[rt] = 0;
            }
        }
    }

    /**************************** For Test *********************************/
    public static class Right {
        public int[] arr;

        public Right(int[] origin) {
            arr = new int[origin.length + 1];
            for (int i = 0; i < origin.length; i++) {
                arr[i + 1] = origin[i];
            }
        }

        public void update(int L, int R, int C) {
            for (int i = L; i <= R; i++) {
                arr[i] = C;
            }
        }

        public void add(int L, int R, int C) {
            for (int i = L; i <= R; i++) {
                arr[i] += C;
            }
        }

        public long query(int L, int R) {
            long ans = 0;
            for (int i = L; i <= R; i++) {
                ans += arr[i];
            }
            return ans;
        }

    }

    public static int[] genarateRandomArray(int len, int max) {
        int size = (int) (Math.random() * len) + 1;
        int[] origin = new int[size];
        for (int i = 0; i < size; i++) {
            origin[i] = (int) (Math.random() * max) - (int) (Math.random() * max);
        }
        return origin;
    }

    public static boolean test() {
        int len = 100;
        int max = 1000;
        int testTimes = 5000;
        int addOrUpdateTimes = 1000;
        int queryTimes = 500;
        for (int i = 0; i < testTimes; i++) {
            int[] origin = genarateRandomArray(len, max);
            SegmentTree seg = new SegmentTree(origin);
            int S = 1;
            int N = origin.length;
            int root = 1;
            seg.build(S, N, root);
            Right rig = new Right(origin);
            for (int j = 0; j < addOrUpdateTimes; j++) {
                int num1 = (int) (Math.random() * N) + 1;
                int num2 = (int) (Math.random() * N) + 1;
                int L = Math.min(num1, num2);
                int R = Math.max(num1, num2);
                int C = (int) (Math.random() * max) - (int) (Math.random() * max);
                if (Math.random() < 0.5) {
                    seg.add(L, R, C, S, N, root);
                    rig.add(L, R, C);
                } else {
                    seg.update(L, R, C, S, N, root);
                    rig.update(L, R, C);
                }
            }
            for (int k = 0; k < queryTimes; k++) {
                int num1 = (int) (Math.random() * N) + 1;
                int num2 = (int) (Math.random() * N) + 1;
                int L = Math.min(num1, num2);
                int R = Math.max(num1, num2);
                long ans1 = seg.query(L, R, S, N, root);
                long ans2 = rig.query(L, R);
                if (ans1 != ans2) {
                    return false;
                }
            }
        }
        return true;
    }

    public static void main(String[] args) {
        int[] origin = { 2, 1, 1, 2, 3, 4, 5 };
        SegmentTree seg = new SegmentTree(origin);
        int S = 1; // 整个区间的开始位置，规定从1开始，不从0开始 -> 固定
        int N = origin.length; // 整个区间的结束位置，规定能到N，不是N-1 -> 固定
        int root = 1; // 整棵树的头节点位置，规定是1，不是0 -> 固定
        int L = 2; // 操作区间的开始位置 -> 可变
        int R = 5; // 操作区间的结束位置 -> 可变
        int C = 4; // 要加的数字或者要更新的数字 -> 可变
        // 区间生成，必须在[S,N]整个范围上build
        seg.build(S, N, root);
        System.out.println(seg.query(L, R, S, N, root));
        // 区间修改，可以改变L、R和C的值，其他值不可改变
        seg.add(L, R, C, S, N, root);
        System.out.println(seg.query(L, R, S, N, root));
        // 区间更新，可以改变L、R和C的值，其他值不可改变
        seg.update(L, R, C, S, N, root);
        // 区间查询，可以改变L和R的值，其他值不可改变
        long sum = seg.query(L, R, S, N, root);
        System.out.println(seg.query(L, R, S, N, root));

        System.out.println("对数器测试开始...");
        System.out.println("测试结果 : " + (test() ? "通过" : "未通过"));
    }
}

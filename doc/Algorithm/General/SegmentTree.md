# 线段树
[LeetCode: 307. 区域和检索 - 数组可修改](https://leetcode-cn.com/problems/range-sum-query-mutable/)的官方描述中看到的这种线段树德构造方式
## 自上而下构造线段树



## 自下而上构造线段树

# 练习题
- [307. 区域和检索 - 数组可修改](https://leetcode-cn.com/problems/range-sum-query-mutable/)
```C++
private void buildTree(int[] nums) {
    for (int i = n, j = 0;  i < 2 * n; i++,  j++)
        tree[i] = nums[j];
    for (int i = n - 1; i > 0; --i)
        tree[i] = tree[i * 2] + tree[i * 2 + 1];
}

void update(int pos, int val) {
    pos += n;
    tree[pos] = val;
    while (pos > 0) {
        int left = pos, right = pos;
        if (pos % 2 == 0) right = pos + 1;
        else left = pos - 1;
        tree[pos / 2] = tree[left] + tree[right]; // udapte parent
        pos /= 2;
    }
}
```
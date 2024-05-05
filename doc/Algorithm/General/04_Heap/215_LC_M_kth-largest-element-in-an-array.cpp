/**
 *  215. 数组中的第K个最大元素
 *  https://leetcode.cn/problems/kth-largest-element-in-an-array/description/
 * 
 *  实现方式：
 *      1. 小顶堆(时间复杂度O(Nlogk)), 空间复杂度O(K)
 *      2. 基于快排
 *      3. 基于堆排
 */

// 1. 小顶堆(时间复杂度O(Nlogk)), 空间复杂度O(K)
int findKthLargest(vector<int>& nums, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_heap;
    for (int i = 0; i < nums.size(); ++i) {
        if (min_heap.size() < k) {
            min_heap.push(nums[i]);
        } else {
            if (nums[i] > min_heap.top()) {
                min_heap.pop();
                min_heap.push(nums[i]);
            }
        }
    }
    return min_heap.top();
}

// 2. 基于快排实现
int quickSelect(std::vector<int>& nums, int l, int r, int k) {
    if (l == r) return nums[l];

    int i = l - 1, j = r + 1, x = nums[random() % (r - l + 1) + l];
    while (i < j) {
        while (nums[++i] > x);
        while (nums[--j] < x);
        if (i < j) std::swap(nums[i], nums[j]);
    }
    if (j - l + 1 >= k) {
        return quickSelect(nums, l, j, k);
    } else {
        return quickSelect(nums, j + 1, r, k - (j - l + 1));
    }
}

int findKthLargest(vector<int>& nums, int k) {
    return quickSelect(nums, 0, nums.size() - 1, k);
}

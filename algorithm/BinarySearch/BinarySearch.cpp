#include<vector>

/**
 * @brief       二分查找实现
 * 
 * @param nums 
 * @param target 
 * @return int 
 */
int search(const std::vector<int>& nums, const int target) {
    int left = 0, right = nums.size() - 1;
    while (left <= right) {
        int middle = left + ((right - left) >> 1);
        if (nums[middle] > target) {
            right = middle - 1;
        } else if (nums[middle] < target) {
            left = middle + 1;
        } else {
            return middle;
        }
    }
    return -1;
}

int main(int argc, char* argv[]) {
    return 0;
}
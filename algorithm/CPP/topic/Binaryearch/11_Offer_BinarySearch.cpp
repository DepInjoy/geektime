#include <vector>
using namespace std;
/**
*   剑指 Offer II 068. 查找插入位置
*   https://leetcode.cn/problems/N6YdxV/description/
*   
*   给定一个排序的整数数组nums和一个整数目标值 target, 请在数组中找到target,
*   并返回其下标。如果目标值不存在于数组中，返回它将会被按顺序插入的位置。
*/
int searchInsert(vector<int>& nums, int target) {
    int l = 0, r= nums.size() - 1;
    int ans = nums.size();
    while (l <= r) {
        int mid = ((r - l) >> 1) + l;
        if (nums[mid] >= target) {
            ans = mid;
            r = mid - 1;
        } else {
            l = mid + 1; 
        }
    }
    return ans;
}

/**
 * 剑指 Offer II 069. 山峰数组的顶部
 *      https://leetcode.cn/problems/B1IidL/description/
 *      https://leetcode-cn.com/problems/peak-index-in-a-mountain-array/
*/
int peakIndexInMountainArray(vector<int>& arr) {
    int n = arr.size();
    int l = 0, r = n - 1;
    int ans = 0;
    while (l <= r) {
        int mid = ((r - l) >> 1) + l;
        if (arr[mid] > arr[mid+1]) {
            ans = mid;
            r = mid - 1;
        } else {
            l = mid + 1;
        }
    }
    return ans;
}

/**
 * 剑指 Offer II 070. 排序数组中只出现一次的数字
 *      https://leetcode.cn/problems/skFtm2/description/
 *      https://leetcode-cn.com/problems/single-element-in-a-sorted-array/
*/
int singleNonDuplicate(vector<int>& nums) {
    int l = 0, r = nums.size() - 1;
    int ans;
    while (l < r) {
        int mid = ((r - l) >> 1) + l;
        if (nums[mid] == nums[mid ^ 1]) {
            l = mid + 1;
        } else {
            r = mid;
        }
    }
    return nums[l];
}

/**
 *  剑指 Offer II 001. 整数除法
 *      https://leetcode.cn/problems/xoh6Oh/description
*/

int divide(int a, int b) {
    if (a == INT_MIN) {
        if (b == 1) return INT_MIN;
        if (b == -1) return INT_MAX;
    }
    if (b == INT_MIN) return a == INT_MIN ? 1 : 0;
    if (a == 0) return 0;

    bool flag = false;
    if (a > 0) {
        a = -a;
        flag = !flag;
    }

    if (b > 0) {
        b = -b;
        flag = !flag;
    }

    int ans = 0;
    while(a <= b) {
        int times = 1, div = b;
        while(div >= (INT_MIN >> 1) && a <= div + div) {
            div += div;
            times += times;
        }
        ans += times;
        a -= div;
    }
    return flag ? -ans : ans;
}

// 快速乘法原理，不懂
int divide(int a, int b) {
    if (a == INT_MIN) {
        if (b == 1) return INT_MIN;
        if (b == -1) return INT_MAX;
    }

    if (b == INT_MIN) return a == INT_MIN ? 1 : 0;

    bool flag = false;
    if (a > 0) {
        a = -a, flag = !flag;
    }

    if (b > 0) {
        b = -b, flag = !flag;
    }

    // a 和 b 是负数，times是正数
    // 需要判断 times * b >= a 是否成立
    auto quickAdd = [](int a, int b, int times) {
        int add = b, ans = 0;
        while(times > 0) {
            if (times & 0x1) {
                // 需要保证 ans + add >= x
                if (a - add > ans) {
                    return false;
                }
                ans += add;
            }
            if (times != 1) {
                // 需要保证 add + add >= x
                if (a - add > add) {
                    return false;
                }
                add += add;
            }
            times >>= 1;
        }
        return true;
    };

    int left = 0, right = INT_MAX, ans = 0;
    while(left <= right) {
        int mid = ((right - left) >> 1) + left;
        bool lessed = quickAdd(a, b, mid);
        if (lessed) {
            ans = mid;
            if (ans == INT_MAX) {
                break;
            }
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return flag ? -ans : ans;
}

/**
 * 50. Pow(x, n)
 *  https://leetcode.cn/problems/powx-n/description/
*/
// 快速幂+递归
double myPow(double x, int n) {
    if (n == 0) return 1;
    long long pn = n;
    if (pn < 0) {
        x = 1 / x;
        pn *= -1;
    }
    double y = myPow(x, pn/2);
    return (pn & 0x1) ? y * y * x : y * y;
}

// 快速幂+迭代
double myPow(double x, int n) {
    // 采用long long 防止n=MIN_INT溢出
    long long pn = n;
    double ans = 1;
    if (pn < 0) {
        x = 1 / x;
        pn *= -1;
    }
    while (pn > 0) {
        if (pn & 0x1) {
            ans *= x;
        }
        x *= x;
        pn >>= 1;
    }
    return ans;
}
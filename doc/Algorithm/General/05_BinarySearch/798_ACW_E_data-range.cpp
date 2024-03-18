/**
 * 789. 数的范围
 * https://www.acwing.com/problem/content/791/
 * 
 * 题目描述:
 * 给定一个按照升序排列的长度为n的整数数组以及q个查询
 * 对于每个查询，返回一个元素k的起始位置和终止位置（位置从0开始计数)
 * 如果数组中不存在该元素, 则返回 -1 -1
*/

#include <iostream>
#include <vector>

int main() {
    int n, q;
    scanf("%d%d", &n, &q);
    std::vector<int> d(n);
    for (int i = 0; i < n; ++i) {
        scanf("%d", &d[i]);
    }
    
    while (q--) {
        int target;
        scanf("%d", &target);
        
        int l = 0, r = n-1;
        while (l < r) {
            int mid = l + r >> 1;
            if (d[mid] >= target) r = mid;
            else l = mid  + 1;
        }
        
        if (d[l] != target) {
            std::cout << "-1 -1" << std::endl;
            continue;
        }
        std::cout << l << " ";
        
        l = 0, r = n - 1;
        while (l < r) {
            int mid  = l + r + 1>> 1;
            if (d[mid] <= target) l = mid;
            else r = mid - 1;
        }
        std::cout << (d[l] == target ? l : -1) << std::endl;
    }
    
    return 0;
}
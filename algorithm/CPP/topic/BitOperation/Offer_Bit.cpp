/**
 * @brief 
 * 
 */
#include <iostream>

/**
 * @brief   剑指Offer-15. 二进制中1的个数
 *          编写一个函数，输入是一个无符号整数（以二进制串的形式），
 *          返回其二进制表达式中数字位数为 '1' 的个数（也被称为 汉明重量).）
 *          
 *          https://leetcode.cn/problems/er-jin-zhi-zhong-1de-ge-shu-lcof/description 
 */

// 如果入参是有符号数据，如果输入是负数，右移后的最高位会设置为1
// 如果一直做右移运算，最终这个数字会变成0xFFFFFFFF而陷入死循环
int hammingWeight(uint32_t n) {
    int weight = 0;
    for (; n; n = n >> 1) {
        weight += ((n & 1) ? 1 : 0);
    }
    return weight;
}

int hammingWeight(uint32_t n) {
    int weight = 0;
    uint32_t flag = 1;
    while (flag) {
        weight += ((n & flag)? 1 : 0);
        flag = flag << 1;
    }
    return weight;
}

int hammingWeight(uint32_t n) {
    int weight = 0;
    while (n) {
        n &= n-1;
        ++weight;
    }
    return weight;
}
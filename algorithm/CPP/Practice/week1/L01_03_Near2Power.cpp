/**
 * @brief       给定一个非负整数num，如何不用循环语句，
 *              返回>=num，并且离num最近的，2的某次方
 * 
 *              实现思想: 将所有的数字变成1再加1
 *              即 10101 -> 11111 -> 11111 + 1 = 100000
 */

#include <iostream>

int tableSizeFor1(int n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n < 0 ? 1 : n + 1;
}

int tableSizeFor(int n) {
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n <= 0 ? 1 : n + 1;
}

int main(int argc, char* argv[]) {
    std::cout << tableSizeFor(0) << std::endl;
    std::cout << tableSizeFor(120) << std::endl;
    std::cout << tableSizeFor(1022) << std::endl;
    return 0;
}
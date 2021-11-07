#include <ranges>
#include <vector>
#include <iostream>
/*
    本文件是针对C++20新特性的一些示例
    编译指令g++-10 -o CPP20 CPP20.cpp -std=c++20
*/

/*  @func: ranges特性的使用示例，假设输入一串数字字从中找到偶数并对其求平方
           参考ranges官方介绍:https://en.cppreference.com/w/cpp/ranges
*/
void rangs_eg_event_square(const std::vector<int> ints) {
    auto even = [](int i) { return 0 == i % 2; };
    auto square = [](int i) { return i * i; };
 
    // "pipe" syntax of composing the views:
    for (int i : ints | std::views::filter(even) | std::views::transform(square)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
 
    // a traditional "functional" composing syntax:
    for (int i : std::views::transform(std::views::filter(ints, even), square)) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::vector<int> ints = {0, 1, 2, 3, 4, 5, 6, 7};
    event_square(ints);
    return 0;
}
/**
 * 生成汇编文件: g++ -S 00_perf_switch.cpp
*/

#include <iostream>
#include <vector>


int main(int argc, char* argv[]) {
    int index;
    std::cin >> index;

    switch (index) {
        case 1:
            std::cout << "Mondy" << std::endl;
            break;
        case 2:
            std::cout << "Tuesday" << std::endl;
            break;
        case 3:
            std::cout << "Wednesday" << std::endl;
            break;
        case 4:
            std::cout << "Thursday" << std::endl;
            break;
        case 5:
            std::cout << "Friday" << std::endl;
            break;
        case 6:
            std::cout << "Saturday" << std::endl;
            break;
        case 7:
            std::cout << "Sunday" << std::endl;
            break;
        default:
            std::cout << "Invalid index" << std::endl;
            break;
    }

    std::vector<int> vec;
    vec.resize(3);
    vec[0] = 0, vec[1] = 1, vec[2] = 2;
    for (int val : vec) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    return 0;
}
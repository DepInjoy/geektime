/**
 * @brief   模板编程采用的是实例化来实现多态，在实际的编程实现有两种实现方案
 *          采用DECLARE_IMPL_SEPARATION(添加宏定义采用声明和实现分离)进行条件编译
 *          1. 将模板函数的实现也放在头文件中
 *              MaxInc.h中存放声明和实现
 *              编译: g++ -o FunctionTemplate FunctionTemplate.cpp
 *          2. 采取声明和实现分离的方案
 *              Max.h进行声明，Max.cpp进行实现
 *              编译: g++ -o FunctionTemplate FunctionTemplate.cpp Max.cpp
 */

#define DECLARE_IMPL_SEPARATION

#include <iostream>
#ifdef DECLARE_IMPL_SEPARATION
#include "Max.h"
#else
#include "MaxInc.h"
#endif

int main(int argc, char* argv[]) {
    int i1 = 42, i2 = 8;
    std::cout << "max(i1, i2):  " << Max(i1, i2) << std::endl;

    double f1 = 3.4, f2 = -6.7;
    std::cout << "max(f1, f2):  " << Max(f1, f2) <<std::endl;

    std::string s1 = "mathematics", s2 = "math";
    std::cout << "max(s1, s2):  " << Max(s1, s2) <<std::endl;
    return 0;
}

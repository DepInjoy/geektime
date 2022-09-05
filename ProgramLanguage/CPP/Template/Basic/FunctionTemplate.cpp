#define DECLARE_IMPL_SEPARATION

#include <iostream>
#ifdef DECLARE_IMPL_SEPARATION
#include "Max.h"
#else
#include "MaxInc.h"
#endif

// https://www.cnblogs.com/zpcdbky/p/16329886.html

int main(int argc, char* argv[]) {
    int i1 = 42, i2 = 8;
    std::cout << "max(i1, i2):  " << Max(i1, i2) << std::endl;

    double f1 = 3.4, f2 = -6.7;
    std::cout << "max(f1, f2):  " << Max(f1, f2) <<std::endl;

    std::string s1 = "mathematics", s2 = "math";
    std::cout << "max(s1, s2):  " << Max(s1, s2) <<std::endl;
    return 0;
}

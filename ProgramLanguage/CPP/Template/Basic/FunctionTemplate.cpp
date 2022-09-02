#include <iostream>
#include "Add.h"

int main(int argc, char* argv[]) {
    int res = Add<int>(1, 2);
    std::cout << "res = " << res << std::endl;
    return 0;
}
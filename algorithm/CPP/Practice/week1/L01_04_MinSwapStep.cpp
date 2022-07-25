/**
 * @brief       一个数组中只有两种字符'G'和’B’，
 *              可以让所有的G都放在左侧，所有的B都放在右侧
 *              或者可以让所有的G都放在右侧，所有的B都放在左侧
 *              但是只能在相邻字符之间进行交换操作，返回至少需要交换几次
 * 
 */
#include <string>
#include <iostream>

int minSteps(const std::string& s) {
    int step1 = 0, step2 = 0;
    int gi = 0, bi = 0;
    const char* str = s.c_str();
    for (int i = 0; i < s.size(); i++) {
        if (str[i] == 'G') {
            step1 += i - (gi++);
        } else {
            step2 += i - (bi++);
        }
    }
    return std::min(step1, step2);
}

int main(int argc, char* argv[]) {
    std::cout << minSteps("GGGGGGGGGGGG") << std::endl;
    std::cout << minSteps("BBBB") << std::endl;
    std::cout << minSteps("GGGGGBGGGGG") << std::endl;
    return 0;
}
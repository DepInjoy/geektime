/**
 * @brief   条款5：对定制的“类型转换函数”保持警
 * 
 */
#include <iostream>

class Rational {
public:
    Rational(int numerator = 0, int denominato = 1):
        numerator_(numerator), denominato_(denominato) {
    }

    operator double() const {
        return static_cast<double>(numerator_) / denominato_;
    }

private:
    int numerator_;
    int denominato_;
};

class Rational2 {
public:
    Rational2(int numerator = 0, int denominato = 1):
        numerator_(numerator), denominato_(denominato) {
    }

    // t
    double asDouble() const {
        return static_cast<double>(numerator_) / denominato_;
    }

private:
    int numerator_;
    int denominato_;
};

int main(int argc, char* argv[]) {
    Rational r(1, 2);
    double tmp = 0.5 * r;  // 将r隐式转换为double再进行乘法
    std::cout << tmp << std::endl;
    // 存在隐式转换会将r隐式转换为double输出，可能实际的意图是调用Rational的operator<<
    std::cout << r << std::endl;

    Rational2 r2(1, 2);
    std::cout << r2.asDouble() << std::endl;
    // 没有实现Rational的operator<<下面的语句执行会报错
    // std::cout << r2 << std::endl;
    return 0;
}

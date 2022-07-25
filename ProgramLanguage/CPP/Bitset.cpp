#include <iostream>
#include <bitset>
#include <limits>
#include <string>

void BitsetAPIExample() {
    // 1. 构造
    std::bitset<16> bset1;
    std::bitset<16> bset2(3);
    std::bitset<16> bset3("1011111011");
    std::cout << "1: " << bset1 << " 2: " << bset2 << " 3: " << bset3 << std::endl;

    // 2. 位运算
    std::bitset<8> bset4(8);
    bset4[0] = 1, bset4[1] = 1;
    std::cout << "After change 0 and 1 bit " << bset4 << std::endl;
    bset4 |= 0b10000;
    std::cout << "After change |= 5 bit " << bset4 << std::endl;
    bset4 &= ~0b10000;
    std::cout << "After change &= 5 bit " << bset4 << std::endl;
    bset4.set(4, 10);
    std::cout << "After change set 5 bit as 1 " << bset4 << std::endl;

    // 3. reset清零/flip位取反
    bset4.reset();
    std::cout << "After reset " << bset4 << std::endl;
    bset4.flip(0);
    std::cout << "After flip " << bset4 << std::endl;

    // 4. 转换为和string
    std::bitset<8> bset5(255);
    std::cout << bset5.to_ullong() << " " << bset5.to_string() << std::endl;

    // 5. count/all/any/none接口
    //      count: bit位数
    //      all 所有位都为1返回true,否则false
    //      any 存在1位为1返回true,否则false
    //      none 所有位都为0返回true,否则false
    std::cout << "count: " << bset1.count() << std::endl;
    std::cout << "all: " << bset1.all() << " any:" << bset1.any() << " none:"<< bset1.none() << std::endl;
    std::cout << "all: " << bset4.all() << " any:" << bset4.any() << " none:"<< bset4.none() << std::endl;
    std::cout << "all: " << bset5.all() << " any:" << bset5.any() << " none:"<< bset5.none() << std::endl;
}

/**
 * @brief   将bitset当作一组标志来管理Flag示例, 其中
 *          每1bit代表一种color
 */
void UseBitsetAsFlag(void) {
    // enumeration type for the bits, each bit represents a color
    enum Color { red, yellow, green, blue, white, black, numColors };
    // create bitset for all bits/colors
    std::bitset<numColors>  usedColors;

    // set bits for two colors
    usedColors.set(red);
    usedColors.set(blue);

    // print some bitset data
    std::cout << "bitfield of used colors:   " << usedColors << std::endl;
    std::cout << "number   of used colors:   " << usedColors.count() << std::endl;
    std::cout << "bitfield of unused colors: " << ~usedColors << std::endl;
    if (usedColors.any()) {  // loop over all colors
        for (int c = 0; c < numColors; ++c) {
            if (usedColors[(Color)c]) {
                std::cout << c << " is actual used, please do something here" << std::endl;
            }
        }
    }
}

/**
 * @brief   数据和bit之间数据的转化
 *          int, string -> bits and bits -> int, string
 */
void Type2Bits() {
    std::cout << "267 as binary uint32_t:       "
        << std::bitset<std::numeric_limits<uint32_t>::digits>(267) << std::endl;
    std::cout << "10,000,000 with 24 bits: " << std::bitset<24>(1e7) << std::endl;
    std::string s = std::bitset<42>(12345678).to_string();
    std::cout << "12,345,678 with 42 bits: " << s << std::endl;
    std::cout << "\"1000101011\" as number:  " << std::bitset<100>("1000101011").to_ullong() <<std::endl;
}

int main(int argc, char* argv[]) {
    BitsetAPIExample();
    std::cout << std::endl << std::endl;
    UseBitsetAsFlag();
    std::cout << std::endl << std::endl;
    Type2Bits();
    return 0;
}

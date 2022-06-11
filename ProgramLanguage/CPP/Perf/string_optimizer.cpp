#include "TimeCost.h"
#include <string>
#include <sstream>

// https://xuhss.com/c%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/c04

std::string remove_ctrl(const std::string s) {
    std::string result;
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] >= 0x20) result = result + s[i];
    }
    return result;
}

/**
 * @brief   采用符合复制操作符+=优化
 *          
 *          移除为分配临时字符串保存连接结果造成的频繁内存管理调用
 *          以及临时字符串复制和删除操作
 */
std::string remove_ctrl_mutating(const std::string s) {
    std::string result;
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] >= 0x20) result += s[i];
    }
    return result;
}

/**
 * @brief   预留存储空间
 *          移除了字符串缓冲区的重新分配
 *          同时改善函数读取数据的缓存局部性(cache locality)
 */
std::string remove_ctrl_reserve(const std::string s) {
    std::string result;
    result.reserve(s.length());
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] >= 0x20) result += s[i];
    }
    return result;
}

/**
 * @brief   移除实参复制
 * 
 *          减少复制输入参数和内存分配的开销
 *          引用变量是作为指针实现的,每次出现s的地方都需要解引指针,会增加开销
 */
std::string remove_ctrl_ref_args(std::string const& s) {
    std::string result;
    result.reserve(s.length());
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] >= 0x20) result += s[i];
    }
    return result;
}

/**
 * @brief   迭代器消除指针解引
 *          字符串迭代器是指向字符缓冲 区的简单指针，节省解引开销
 */
std::string remove_ctrl_ref_args_it(const std::string& s) {
    std::string result;
    result.reserve(s.length());
    for (auto it = s.begin(), end = s.end(); it != end; ++it) {
        if (*it >= 0x20) result += *it;
    }
    return result;
}

/**
 * @brief   消除对返回的字符串的复制
 */
void remove_ctrl_ref_result_it(const std::string& s, std::string& result) {
    result.clear();
    result.reserve(s.length());
    for (auto it = s.begin(), end = s.end(); it != end; ++it) {
        if (*it >= 0x20) result += *it;
    }
}

/**
 * @brief   用字符数组代替字符串
 */
void remove_ctrl_cstrings(char* dest, char const* src, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (src[i] >= 0x20) *dest++ = src[i];
    }
    *dest = '\0';
}


void stringTest1(const uint64_t times) {
    std::string s("\07Now is the time\07 for all good men\r\n to come to the aid of their country. \07");

    /***************** 通过降低内存分配降低开销 *****************/
    {
        TimeCost timeCost("remove_ctrl");
        for (int i = 0; i < times; i++) {
            remove_ctrl(s);
        }
    }

    {
        TimeCost timeCost("remove_ctrl_mutating");
        for (int i = 0; i < times; i++) {
            remove_ctrl_mutating(s);
        }
    }

    {
        TimeCost timeCost("remove_ctrl_reserve");
        for (int i = 0; i < times; i++) {
            remove_ctrl_reserve(s);
        }
    }

    /********************* 通过降低复制操作降低开销 **********************/
    {
        TimeCost timeCost("remove_ctrl_ref_args");
        for (int i = 0; i < times; i++) {
            remove_ctrl_ref_args(s);
        }
    }

    {
        TimeCost timeCost("remove_ctrl_ref_args_it");
        for (int i = 0; i < times; i++) {
            remove_ctrl_ref_args_it(s);
        }
    }

    {
        TimeCost timeCost("remove_ctrl_ref_result_it");
        std::string result;
        for (int i = 0; i < times; i++) {
            remove_ctrl_ref_result_it(s, result);
        }
    }

    {
        TimeCost timeCost("remove_ctrl_cstrings");
        char* dest = (char*)malloc(s.size());
        for (int i = 0; i < times; i++) {
            remove_ctrl_cstrings(dest, s.c_str(), s.size());
        }
        free(dest);
    }
}

/**
 * @brief   改进算法
 * 
 *          一次将一个字符复制到结果字符串,会导致差的内存分配
 *          通过将整个子字符串移动至结果字符串中改善性能
 */
std::string remove_ctrl_block(const std::string& s) {
    std::string result;
    for (size_t b = 0, i = b, e = s.length(); b < e; b = i+1) {
        for (i = b; i < e; ++i) {
            if (s[i] < 0x20) break;
        }
        // substr仍然会生成临时字符串
        result += s.substr(b, i - b);
    }
    return result;
}

/**
 * @brief   更快的算法
 *          通过std::string的append()成员函数来复制子字符串，且无需创建临时字符串
 */
std::string remove_ctrl_block_append(const std::string& s) {
    std::string result;
    result.reserve(s.length());
    for (size_t b = 0, i = b, e = s.length(); b < e; b = i+1) {
        for (i = b; i < e; ++i) {
            if (s[i] < 0x20) break;
        }
        result.append(s, b, i - b);
    }
    return result;
}

/**
 * @brief   使用std::string的erase()成员函数移除控制字符来改变字符串
 *          
 *          不创建新的字符串，而是修改参数字符串的值作为结果返回
 *          只在返回时发生内存分配，其他情况无内存分配
 */
std::string remove_ctrl_erase(std::string s) {
    for (size_t i = 0; i < s.length();)
        if (s[i] < 0x20) {
            s.erase(i, 1);
        } else {
            ++i;
        }
    return s;
}

void stringTest2(const uint64_t times) {
    std::string s("\07Now is the time\07 for all good men\r\n to come to the aid of their country. \07");

    {
        TimeCost timeCost("remove_ctrl_block");
        for (int i = 0; i < times; i++) {
            remove_ctrl_block(s);
        }
    }

    {
        TimeCost timeCost("remove_ctrl_block_append");
        for (int i = 0; i < times; i++) {
            remove_ctrl_block_append(s);
        }
    }

    {
        TimeCost timeCost("remove_ctrl_erase");
        for (int i = 0; i < times; i++) {
            remove_ctrl_erase(s);
        }
    }

    {
        TimeCost timeCost("remove_ctrl_stringstream");
        for (int i = 0; i < times; i++) {
            std::string result = remove_ctrl_stringstream(s);
            if (i == 0) std::cout << result << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    const uint64_t testTimes = 100000;
    stringTest1(testTimes);
    stringTest2(testTimes);
    return 0;
}
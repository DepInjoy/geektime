#include <iostream>

/*
    内部排序算法实现
*/

/**
 * @brief       只是用于验证时，将数据打印便于对结果的确认
 * 
 * @param array 待打印的数组
 * @param N     待打印的数组中数据个数
 */
void printData(const int array[], const int N) {
    for (int i = 0; i < N; ++i) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

template<typename T>
void Swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

/**
 * @brief       冒泡排序算法
 * 
 * @tparam T    待排序的数据的数据类型
 * @param data  待排序的数据
 * @param N     待排序的数据的个数
 */
template<typename T>
void Bubble_Sort(T data[], const int N) {
    for (int i = N - 1; i >= 0; --i) {
        // 发生交换标志位，如果全程无交换，则表明数据已经是排序好的，可终止排序
        bool flag = false;
        for (int j = 0; j < i; ++j) {
            if (data[j + 1] < data[j]) {  // 一趟冒泡
                Swap(data[j + 1], data[j]);
                flag = true;  // 数据发生交换标志
            }
        }
        // 全程无交换，终止排序，减少无效的操作
        if (!flag) break;
    }
}
/**
 * @brief       插入排序算法
 * 
 * @tparam T    
 * @param data 
 * @param N 
 */
template<typename T>
void Inseter_Sort(T data[], const int N) {
    for (int i = 1; i < N; ++i) {
        T tmp = data[i];  // 抽到一张牌
        for (int j = i; j >= 0 && tmp < data[j]; --j) {  // 找到这张牌的应该放的位置
            data[j - 1] = data[j];  // 腾挪位置
            data[j] = tmp;  // 新牌落位
        }
    }
}

int main(int argc, char* argv[]) {
    const int N = 11;  // 用于验证的数据的个数
    int array[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};  // 用于验证的数组
    Bubble_Sort(array, N);
    printData(array, N);

    Inseter_Sort(array, N);
    printData(array, N);
    return 0;
}

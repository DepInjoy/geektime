#include <iostream>
#include <unordered_map>

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
void Inserter_Sort(T data[], const int N) {
    for (int i = 1; i < N; ++i) {
        T tmp = data[i];  // 抽到一张牌
        for (int j = i; j >= 0 && tmp < data[j]; --j) {  // 找到这张牌的应该放的位置
            data[j - 1] = data[j];  // 腾挪位置
            data[j] = tmp;  // 新牌落位
        }
    }
}

/**
 * @brief       原始希尔排序，D(M)=floor(N/2), D(k)=floor(D(k+1)/2)
 * 
 * @tparam T 
 * @param data 
 * @param N 
 */
template<typename T>
void Origin_Shell_Sort(T data[], const int N) {
    for (int D = N / 2; D > 0; D /= 2) {  // 希尔增量序列
        T tmp = data[D];
        for (int i = D; i < N && tmp < data[i]; i += D) {  // 插入排序
            data[i - D] = data[i];
            data[i] = tmp;
        }
    }
}

template<typename T>
void Sedgewick_Shell_Sort(T data[], const int N) {
    // Sedgewick部分增量
    int Sedgewick[] = {929, 505, 209, 109, 41, 19, 5, 1, 0};
    for (int si = 0; Sedgewick[si] < N; ++si) {
        for (int D = Sedgewick[si]; D > 0; D = Sedgewick[si+1]) {  // Sedgewick增量序列
            T tmp = data[D];
            for (int i = D; i < N && tmp < data[i]; i += D) {  // 插入排序
                data[i - D] = data[i];
                data[i] = tmp;
            }
        }
    }
}

template<typename T>
int ScanForMinIndex(T data[], int start, int end) {
    int index = start;
    for (int i = start; i < end; ++i) {
        if (data[i] < data[index]) {
            index = i;
        }
    }
    return index;
}

/**
 * @brief       选择排序实现
 * 
 * @tparam T 
 * @param data 
 * @param N 
 */
template<typename T>
void Selection_Sort(T data[], const int N) {
    for (int i = 0; i < N; ++i) {
        // 寻找未排序部分(data[i]到data[N-1])中最小值所在位置,记为min_index
        int min_index = ScanForMinIndex(data, i, N - 1);
        // 将未排序部分的最小元换到有序部分的最后位置
        Swap(data[min_index], data[i]);
    }
}

int main(int argc, char* argv[]) {
    const int N = 11;  // 用于验证的数据的个数
    int array[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};  // 用于验证的数组
    std::cout << "Bubble Sort:" << std::endl;
    Bubble_Sort(array, N);
    printData(array, N);

    std::cout << "Inserter Sort:" << std::endl;
    Inserter_Sort(array, N);
    printData(array, N);

    std::cout << "Origin Shell Sort:" << std::endl;
    Origin_Shell_Sort(array, N);
    printData(array, N);

    std::cout << "Sedgewick Shell Sort:" << std::endl;
    Sedgewick_Shell_Sort(array, N);
    printData(array, N);

    std::cout << "Selection Shell Sort:" << std::endl;
    Selection_Sort(array, N);
    printData(array, N);
    return 0;
}

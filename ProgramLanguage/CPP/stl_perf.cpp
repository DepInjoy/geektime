#include <array>
#include <vector>
#include <list>
#include <set>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>

class BigInt {
public:
        // BigInt(const char *);
        BigInt (unsigned u = 0) {
            unsigned v = u;
            for (ndigits = 1; (v/=10) > 0; ++ndigits) {;// 计算整数u的位数
            }
            digits = new char[size=ndigits];
            for ( unsigned i = 0; i < ndigits; ++i) {
                digits[i] = u%10;  // 将整数u的每位数分开
                u /= 10;
            }
        }

        BigInt (const BigInt& copyFrom) {  //  拷贝构造
            size = ndigits = copyFrom.ndigits;
            digits = new char[size];
            for ( unsigned i = 0; i < ndigits; ++i) {
                digits[i] = copyFrom.digits[i];
            }
        }

        ~BigInt () {
            delete[] digits;
        }
private:
    char*       digits;
    unsigned    ndigits;
    unsigned    size;
};

template <class T>
void arrayInsert(T *a, T *collection, int size) {
    for (int k =0; k < size; k++) {
        a[k] = collection[k];
    }
}
template <class T>
void vectorInsert(std::vector<T> *v, T *collection, int size) {
    for (int k =0; k < size; k++) {
        v->push_back(collection[k]);
    }
}

template <class T>
void listInsert(std::list<T> *l, T *collection, int size) {
    for (int k =0; k < size; k++) {
        l->push_back(collection[k]);
    }
}

template <class T>
void multisetInsert(std::multiset<T> *s, T *collection, int size) {
    for (int k =0; k < size; k++) {
        s->insert(collection[k]);
    }
}

int* genIntData(int size) {
    int *data = new int[size];  //  生成随机整数并将它们保存到data数组
    std::generate(&data[0], &data[size], std::rand);
    return data;
}

BigInt* genBigIntData(int size) {
    size = 1000;
    std::array<BigInt, 1000> data; //  生成随机整数并将它们保存到data数组
    for (int i = 0; i < size; i++) {
        data[i] = BigInt(std::rand());
    }
    std::cout << "genBigIntData [OUT]" << std::endl;
    return data.data();
}

/**
 * @brief:  STL中插入100万int数据, 性能测试
*/
void stl_inster_int_perf_test(void) {
    const int test_data_size = 1000000;
    const int test_times = 100;
    std::vector<std::vector<int> > vv_elapsed_ms(4);
    for(int time = 0; time < test_times; time++) {
        int* test_data = genIntData(test_data_size);

        std::array<int, test_data_size> arr;
        std::vector<int> vect;
        std::list<int> l;
        std::multiset<int> mset;

        std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
        arrayInsert(arr.data(), test_data, test_data_size);
        std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
        uint64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        vv_elapsed_ms[0].push_back(elapsed_ms);
        std::cout << "arrayInsert elapsed time: " << elapsed_ms << " ms" << std::endl;

        start = std::chrono::steady_clock::now();
        vectorInsert(&vect, test_data, test_data_size);
        end = std::chrono::steady_clock::now();
        elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        vv_elapsed_ms[1].push_back(elapsed_ms);
        std::cout << "vectorInsert elapsed time: " << elapsed_ms << " ms" << std::endl;

        start = std::chrono::steady_clock::now();
        listInsert(&l, test_data, test_data_size);
        end = std::chrono::steady_clock::now();
        elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        vv_elapsed_ms[2].push_back(elapsed_ms);
        std::cout << "listInsert elapsed time: " << elapsed_ms << " ms"  << std::endl;

        start = std::chrono::steady_clock::now();
        multisetInsert(&mset, test_data, test_data_size);
        end = std::chrono::steady_clock::now();
        elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        vv_elapsed_ms[3].push_back(elapsed_ms);
        std::cout << "multisetInsert elapsed time: " << elapsed_ms << " ms"  << std::endl;

        delete[] test_data;
        std::cout << std::endl;
    }

    std::cout << "arrayInsert elapsed mean time: " <<
            std::accumulate(vv_elapsed_ms[0].begin(), vv_elapsed_ms[0].end(), 0) / vv_elapsed_ms[0].size()
            << " ms" << std::endl;
    std::cout << "vectorInsert elapsed mean time: " <<
            std::accumulate(vv_elapsed_ms[1].begin(), vv_elapsed_ms[1].end(), 0) / vv_elapsed_ms[1].size()
            << " ms" << std::endl;
    std::cout << "listInsert elapsed mean time: " <<
            std::accumulate(vv_elapsed_ms[2].begin(), vv_elapsed_ms[2].end(), 0) / vv_elapsed_ms[2].size()
            << " ms" << std::endl;
    std::cout << "multisetInsert elapsed mean time: " <<
            std::accumulate(vv_elapsed_ms[3].begin(), vv_elapsed_ms[3].end(), 0) / vv_elapsed_ms[3].size()
            << " ms" << std::endl;
}

void stl_inster_bigint_perf_test(void) {
    const int test_data_size = 1000;
    const int test_times = 1;
    std::vector<std::vector<int> > vv_elapsed_ms(4);
    for(int time = 0; time < test_times; time++) {
        std::cout << "before gen data" << std::endl;
        BigInt* test_data = genBigIntData(test_data_size);
        std::cout << "after gen data" << std::endl;
        std::array<BigInt, test_data_size> arr;
        std::vector<BigInt> vect;
        std::list<BigInt> l;
        std::multiset<BigInt> mset;
/*
        std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
        arrayInsert(arr.data(), test_data, test_data_size);
        std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
        uint64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        vv_elapsed_ms[0].push_back(elapsed_ms);
        std::cout << "arrayInsert elapsed time: " << elapsed_ms << " ms" << std::endl;

        start = std::chrono::steady_clock::now();
        vectorInsert(&vect, test_data, test_data_size);
        end = std::chrono::steady_clock::now();
        elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        vv_elapsed_ms[1].push_back(elapsed_ms);
        std::cout << "vectorInsert elapsed time: " << elapsed_ms << " ms" << std::endl;

        start = std::chrono::steady_clock::now();
        listInsert(&l, test_data, test_data_size);
        end = std::chrono::steady_clock::now();
        elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        vv_elapsed_ms[2].push_back(elapsed_ms);
        std::cout << "listInsert elapsed time: " << elapsed_ms << " ms"  << std::endl;

        start = std::chrono::steady_clock::now();
        multisetInsert(&mset, test_data, test_data_size);
        end = std::chrono::steady_clock::now();
        elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        vv_elapsed_ms[3].push_back(elapsed_ms);
        std::cout << "multisetInsert elapsed time: " << elapsed_ms << " ms"  << std::endl;
*/
        std::cout << "here" << std::endl;
        // delete[] test_data;
        std::cout << "here" << std::endl;
    }

    std::cout << "arrayInsert elapsed mean time: " <<
            std::accumulate(vv_elapsed_ms[0].begin(), vv_elapsed_ms[0].end(), 0) / vv_elapsed_ms[0].size()
            << " ms" << std::endl;
    std::cout << "vectorInsert elapsed mean time: " <<
            std::accumulate(vv_elapsed_ms[1].begin(), vv_elapsed_ms[1].end(), 0) / vv_elapsed_ms[1].size()
            << " ms" << std::endl;
    std::cout << "listInsert elapsed mean time: " <<
            std::accumulate(vv_elapsed_ms[2].begin(), vv_elapsed_ms[2].end(), 0) / vv_elapsed_ms[2].size()
            << " ms" << std::endl;
    std::cout << "multisetInsert elapsed mean time: " <<
            std::accumulate(vv_elapsed_ms[3].begin(), vv_elapsed_ms[3].end(), 0) / vv_elapsed_ms[3].size()
            << " ms" << std::endl;
}

int main(int argc, char* argv[]) {
    // stl_inster_int_perf_test();
    stl_inster_bigint_perf_test();
    return 0;
}
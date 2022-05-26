#include <vector>
#include <iostream>
#include <algorithm>
/**
 * @brief   简单排序:冒泡排序
 */
void BubbleSort(std::vector<int>& d) {
    for (int i = d.size()-1; i >= 0; --i) {
        bool flag = false;
        for (int j = 0; j < i; j++) {
            if (d[j] > d[j+1]) {
                std::swap(d[j+1], d[j]);
                flag = true;
            }
        }
        if (!flag) break;
    }
}

/**
 * @brief   简单排序:插入排序
 */
void InsertionSort(std::vector<int>& d) {
    for (int i = 1; i < d.size(); i++) {
        int j = i, tmp = d[i];// 抽到一张牌并寻找新牌摆放的位置
        for (; j > 0 && d[j-1] > tmp; --j) {
            d[j] = d[j-1]; // 腾挪位置
        }
        d[j] = tmp;
    }
}

void quickSort(std::vector<int>& d, const int l, const int r) {
    if (l >= r) return;

    int pivot = d[l+r>>1], i = l-1, j = r+1;
    while (i < j) {
        while (d[++i] < pivot);
        while (d[--j] > pivot);
        if (i < j) std::swap(d[i], d[j]);
    }
    quickSort(d, l, j), quickSort(d, j+1, r);
}

/**
 * @brief       快排
 */
void QuickSort(std::vector<int>& d) {
    quickSort(d, 0, d.size()-1);
}

void MergeSort(std::vector<int>& q, int l, int r) {
    if (l >= r) return;

    std::vector<int> tmp(r-l+1);
    int mid = (l + r) >> 1;
    MergeSort(q, l, mid), MergeSort(q, mid+1, r);

    int i = l, j = mid + 1, k = 0;
    while (i <= mid && j <= r) {
        if (q[i] <= q[j]) tmp[k++] = q[i++];
        else tmp[k++] = q[j++];
    }
    while(i <= mid) tmp[k++] = q[i++];
    while(j <= r) tmp[k++] = q[j++];

    for (i = l, j = 0; i <= r; i++, j++) {
        q[i] = tmp[j];
    }
}
/**
 * @brief   归并排序
 */
void MergeSort(std::vector<int>& d) {
    MergeSort(d, 0, d.size()-1);
}

// 堆排序元素下推
void down(std::vector<int>& heap, const int i, const int size) {
    int t = i;
    if (2*i+1 <= size && heap[2*i+1] > heap[t]) t = 2*i+1;
    if (2*i+2 <= size && heap[2*i+2] > heap[t]) t = 2*i+2;
    if (t != i) {
        std::swap(heap[i], heap[t]);
        down(heap, t, size);
    }
}

// 堆排序
void HeapSort(std::vector<int>& d) {
    int n = d.size();
    // 构建大顶堆
    for (int i = n/2-1; i >= 0; i--) down(d, i, n-1);
    for (int i = n-1; i > 0; i--) {
        std::swap(d[0], d[i]);
        down(d, 0, i-1);
    }
}

/********************** For Test **********************/
void copyArray(const std::vector<int>& d1, std::vector<int>& d2) {
    d2.reserve(d1.size());
    for (int i = 0; i < d1.size(); i++) {
        d2.insert(d2.begin() + i, d1[i]);
    }
}

void printArray(const std::vector<int>& d) {
    for (int i = 0; i < d.size(); i++) {
        std::cout << d[i] << " ";
    }
    std::cout << std::endl;
}

bool isEqual(const std::vector<int>& d1, const std::vector<int>& d2) {
    if (d1.size() != d2.size()) return false;

    for (int i = 0; i < d1.size(); i++) {
        if (d1[i] != d2[i]) return false;
    }
    return true;
}

void generateRandomData(const int dmax, const int len, std::vector<int>& data) {
    for (int i = 0; i < len; i++) {
        int item = std::rand() % (dmax+1);
        data.push_back(item);
    }
}

int main(int argc, char* argv[]) {
    int maxLen = 32;
    int testTimes = 100000;
    int maxData = 1000;

    for (int i = 0; i < testTimes; i++) {
        int maxd = std::rand() % maxData;
        int maxl = std::rand() % maxLen +1;
        std::vector<int> data, sortedData;
        generateRandomData(maxd, maxl, data);
        copyArray(data, sortedData);
        std::sort(sortedData.begin(), sortedData.end());

        std::vector<int> d1, d2, d3, d4, d5;
        copyArray(data, d1);
        QuickSort(d1);

        copyArray(data, d4);
        MergeSort(d4);

        copyArray(data, d5);
        HeapSort(d5);

        copyArray(data, d2);
        BubbleSort(d2);

        copyArray(data, d3);
        InsertionSort(d3);

        if (!isEqual(d1, sortedData) || !isEqual(d2, sortedData) || !isEqual(d3, sortedData)
            || !isEqual(d4, sortedData) || !isEqual(d5, sortedData)) {
            printArray(data);
            std::cout << "Opps, Error" << std::endl;
            return -1;
        }
    }
    return 0;
}

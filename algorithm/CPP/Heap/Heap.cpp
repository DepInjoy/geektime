#include <limits>
#include <vector>
#include <iostream>

struct MaxHeap {
    std::vector<int>        heap;
    int                     capacity;
    int                     heapSize;

    explicit MaxHeap(int capacity) : heap(capacity + 1) {
        // 设置哨兵, 便于后面的快速计算
        heap[0] = std::numeric_limits<int>::max();
        this->capacity = capacity;
        heapSize = 0;
    }

    bool IsExpty() {
        return heapSize == 0;
    }

    bool IsFull() {
        return heapSize == capacity;
    }

    bool Insert(int value) {
        if (IsFull()) {
            std::cout << "Heap is empty" << std::endl;
            return false;
        }

        int i = ++heapSize;
        for (; heap[i/2] < value; i/=2) {
            heap[i] = heap[i/2];
        }
        heap[i] = value;
        return true;
    }

    int DeleteMax() {
        if (IsExpty()) {
            std::cout << "Heap is Empty" << std::endl;
            return std::numeric_limits<int>::max();
        }

        int ans = heap[1];
        int lastItem = heap[heapSize--];
        int parent = 1;
        while (parent*2 <= heapSize) {
            int left = 2 * parent;
            int best = (left + 1 <= heapSize && heap[left + 1] > heap[left]) ? left + 1 : left;
            if (lastItem < heap[best]) {
                heap[parent] = heap[best];
                parent = best;
            } else {
                break;
            }
        }
        heap[parent] = lastItem;
        return ans;
    }
};

int main(int argc, char* argv[]) {
    MaxHeap heap(10);
    heap.Insert(2);
    heap.Insert(8);
    heap.Insert(1);
    heap.Insert(6);
    heap.Insert(7);
    heap.Insert(9);
    heap.Insert(5);
    heap.Insert(4);
    heap.Insert(3);
    heap.Insert(0);
    while (!heap.IsExpty()) {
        std::cout << heap.DeleteMax() << " ";
    }
    std::cout << std::endl;

    return 0;
}
package algorithm.training.struct.Heap;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;

/**
 * @brief:      语言提供的堆结构 vs 手写的堆结构
 *                  取决于，你有没有动态改信息的需求！
 *                  语言提供的堆结构，如果你动态改数据，不保证依然有序
 *                  手写堆结构，因为增加了对象的位置表，所以能够满足动态改信息的需求
 * */
public class C07_HeapGreater {
    /**
     * @brief:      其中T是非基础类型
     * */
    public class HeapGreater<T> {
        private ArrayList<T> heap;
        private HashMap<T, Integer> indexMap;
        private int heapSize;
        private Comparator<? super T> comp;

        public HeapGreater(Comparator<T> c) {
            heap = new ArrayList<T>();
            indexMap = new HashMap<T, Integer>();
            heapSize = 0;
            comp = c;
        }

        public boolean isEmpty() {
            return heapSize == 0;
        }

        public int size() {
            return heapSize;
        }

        public boolean contains(T obj) {
            return indexMap.containsKey(obj);
        }

        public T peek() {
            return heap.get(0);
        }

        public void push(T obj) {
            indexMap.put(obj, heapSize);
            heap.add(obj);
            heapInsert(heapSize++);
        }

        public T pop() {
            T ans = heap.get(0);
            swap(0, heapSize-1);  // 将第一个元素和最后一个元素交换
            indexMap.remove(ans);
            heap.remove(--heapSize);
            heapify(0);
            return ans;
        }

        public void remove(T obj) {
            int index = indexMap.get(obj);
            indexMap.remove(obj);
            T lastObj = heap.get(heapSize-1);
            heap.remove(--heapSize);  // 将最后一个元素删除
            if (index != heapSize) {  // 删除的不是最后一个元素
                // 将最后一个元素调整到index位置
                heap.add(index, lastObj);
                indexMap.put(lastObj, index);
                heapify(index); // 树调整,应该只需要向下调整吧？原代码是resign
            }
        }

        public void resign(T obj) {
            int index = indexMap.get(obj);
            heapInsert(index);
            heapify(index);
        }

        public List<T> getAllElements() {
            List<T> ans = new ArrayList<T>();
            for (T item : heap) {
                ans.add(item);
            }
            return ans;
        }

        private void swap(int index1, int index2) {
            T obj1 = heap.get(index1);
            T obj2 = heap.get(index2);
            indexMap.put(obj1, index2);
            indexMap.put(obj2, index1);
            heap.add(index1, obj2);
            heap.add(index2, obj1);
        }

        private void heapInsert(int index) {
            while (comp.compare(heap.get(index), heap.get((index-1)/2)) < 0) {
                swap(index, (index-1)/2);
                index = (index-1)/2;
            }
        }

        private void heapify(int index) {
            int leftIndex = 2 * index + 1;
            while (leftIndex < heapSize) {
                int best = (leftIndex + 1 < heapSize && comp.compare(
                            heap.get(leftIndex+1), heap.get(leftIndex)) < 0) ?
                        leftIndex + 1 : leftIndex;
                if (comp.compare(heap.get(best), heap.get(index)) < 0) {
                    swap(best, index);
                    leftIndex = 2 * best + 1;
                } else {
                    break;
                }
            }
        }
    }

    public static void main(String[] args) {}
}

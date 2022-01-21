package algorithm.training.struct.Heap;

/***
 * @brief:      堆, 优先级队列就是堆结构
 *              1. 堆结构就是用数组实现的完全二叉树结构
 *              2. 完全二叉树中如果每棵子树的最大值都在顶部就是大根堆
 *              3. 完全二叉树中如果每棵子树的最小值都在顶部就是小根堆
 *              4. 堆结构的heapInsert与heapify操作
 *              5. 堆结构的增大和减少
 */
public class C06_02_Heap {
    static class MaxHeap{
        private int[]       heap;
        private int         heapSize;
        private int         capacity;

        public MaxHeap(int capacity) {
            heap = new int[capacity];
            this.capacity = capacity;
            this.heapSize = 0;
        }

        public boolean isEmpty() {
            return heapSize == 0;
        }

        public boolean isFull() {
            return  heapSize == capacity;
        }

        public void push(int value) {
            if (isFull()) throw new RuntimeException("Heap is full!");

            heap[heapSize] = value;
            heapInsert(heap, heapSize++);
        }

        public int pop() {
            if (isEmpty()) throw new RuntimeException("Heap is empty!");

            int ans = heap[0];
            // 将第一个元素和最后一个元素交换
            swap(heap, 0, --heapSize);
            heapify(heap, 0, heapSize);
            return ans;
        }

        private void swap(int[] arr, int before, int after) {
            int tmp = arr[before];
            arr[before] = arr[after];
            arr[after] = tmp;
        }

        private void heapInsert(int[] arr, int index) {
            while (arr[index] > arr[(index - 1)/2]) {  // 当前节点比父节点大调整
                swap(heap, index, (index-1)/2);
                index = (index-1)/2;
            }
        }

        private void heapify(int[] arr, int index, int heapSize) {
            int leftChildIndex = 2 * index + 1;
            while (leftChildIndex < heapSize) {
                // 寻找左右孩子中最大值所在的索引
                int maxChildIndex = (leftChildIndex + 1 < heapSize &&
                        arr[leftChildIndex] < arr[leftChildIndex + 1]) ? // 存在右孩子且右孩子更大
                        leftChildIndex + 1 : leftChildIndex;
                if (arr[index] < arr[maxChildIndex]) { // 小于孩子节点,交换继续下一层
                    swap(heap, maxChildIndex, index);
                    leftChildIndex = 2 * maxChildIndex + 1;
                } else {
                    break;
                }
            }
        }
    }

    public static void main(String[] args) {
        MaxHeap maxHeap = new MaxHeap(10);
        try {
            assert maxHeap.isEmpty() == true;
            assert maxHeap.isFull() == false;

            maxHeap.push(2);
            maxHeap.push(8);
            maxHeap.push(1);
            maxHeap.push(6);
            maxHeap.push(7);
            maxHeap.push(9);
            maxHeap.push(5);
            maxHeap.push(4);
            maxHeap.push(3);
            maxHeap.push(0);

            maxHeap.push(10);
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        assert maxHeap.pop() == 9;
        assert maxHeap.pop() == 8;
        assert maxHeap.pop() == 7;
        assert maxHeap.pop() == 6;
        assert maxHeap.pop() == 5;
        assert maxHeap.pop() == 4;
        assert maxHeap.pop() == 3;
        assert maxHeap.pop() == 2;
        assert maxHeap.pop() == 1;
    }
}

package algorithm.practice.dc.week2;

import java.util.HashMap;

/**
 * @brief:      设计有setAll功能的哈希表
 *              put、get、setAll方法，时间复杂度O(1)
 *
 * */
public class DC_02_05_SetAll {
    public static class MyValue<V> {
        public V value;
        public long time;

        public MyValue(long time, V value) {
            this.value = value;
            this.time = time;
        }
    }

    public static class MyHashMap<K, V> {
        private long                        curTime;
        private HashMap<K, MyValue<V> >     hashMap;
        private MyValue<V>                  setAllValue;

        public MyHashMap() {
            curTime = 0;
            hashMap = new HashMap<>();
            setAllValue = new MyValue<>(-1, null);
        }

        public void put(K key, V value) {
            hashMap.put(key, new MyValue<>(curTime, value));
            curTime++;
        }

        public void setAll(V value) {
            setAllValue.time = curTime;
            setAllValue.value = value;
        }

        public V get(K key) {
            if (!hashMap.containsKey(key)) return null;

            return (setAllValue.time > hashMap.get(key).time) ?
                    setAllValue.value : hashMap.get(key).value;
        }
    }

    public static void main(String[] args) {
        MyHashMap<Integer, Integer> mmap = new MyHashMap<>();
        System.out.println(mmap.get(0)); // null
        mmap.put(0, 2);
        System.out.println(mmap.get(0));
        mmap.setAll(3);
        System.out.println(mmap.get(0));
        System.out.println(mmap.get(1));
        mmap.put(0, 4);
        System.out.println(mmap.get(0));

        mmap.put(1, 2);
        mmap.put(2, 4);
        mmap.put(3, 8);
        System.out.println("1: " + mmap.get(1) + " 2: " +
                mmap.get(2) + " 3:" + mmap.get(3));
        mmap.setAll(0);
        System.out.println("1: " + mmap.get(1) + " 2: " +
                mmap.get(2) + " 3:" + mmap.get(3));
        mmap.put(3, 8);
        System.out.println("1: " + mmap.get(1) + " 2: " +
                mmap.get(2) + " 3:" + mmap.get(3));
    }
}

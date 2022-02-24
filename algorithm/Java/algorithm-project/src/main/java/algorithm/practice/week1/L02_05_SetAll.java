package algorithm.practice.week1;

import java.util.HashMap;

/**
 * @brief       设计有setAll功能的哈希表
 *              put、get、setAll方法，时间复杂度O(1)
 */

public class L02_05_SetAll {
    public static class MValue<T> {
        public T value;
        public long time;

        public MValue(T value, long time) {
            this.value = value;
            this.time = time;
        }
    }

    public static class MHashMap<K, V> {
        private HashMap<K, MValue<V>>       hashMap;
        private long                        time;
        private MValue<V>                   setAllMValue;

        public MHashMap() {
            hashMap = new HashMap<K, MValue<V>>();
            time = 0;
            setAllMValue = new MValue<>(null, -1);
        }

        public void put(K key, V value) {
            hashMap.put(key, new MValue<V>(value, this.time++));
        }

        public V get(K key) {
            if (!hashMap.containsKey(key)) return null;

            if (hashMap.get(key).time >= setAllMValue.time) {
                return hashMap.get(key).value;
            } else {
                return setAllMValue.value;
            }

        }

        public void setAll(V value) {
            setAllMValue.value = value;
            setAllMValue.time = this.time++;
        }
    }

    public static void main(String[] args) {
        MHashMap myhashMap = new MHashMap<Integer, Integer>();
        myhashMap.put(1, 2);
        myhashMap.put(2, 4);
        myhashMap.put(3, 8);
        System.out.println("1: " + myhashMap.get(1) + " 2: " +
                myhashMap.get(2) + " 3:" + myhashMap.get(3));
        myhashMap.setAll(0);
        System.out.println("1: " + myhashMap.get(1) + " 2: " +
                myhashMap.get(2) + " 3:" + myhashMap.get(3));
        myhashMap.put(3, 8);
        System.out.println("1: " + myhashMap.get(1) + " 2: " +
                myhashMap.get(2) + " 3:" + myhashMap.get(3));
    }
}

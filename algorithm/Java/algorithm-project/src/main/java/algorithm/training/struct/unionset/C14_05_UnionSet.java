package algorithm.training.struct.unionset;

import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Stack;

public class C14_05_UnionSet {
    class Node<V> {
        V       value;

        public Node(V value) {
            this.value = value;
        }
    }

    class UnionSet<V> {
        // 存储集合中的节点
        public HashMap<V, Node<V> >            nodes;
        // 存储节点到根节点的映射，根节点指向自己
        public HashMap<Node<V>, Node<V>>       parents;
        // 存储根节点到集合规模的映射
        public HashMap<Node<V>, Integer>       sizeMap;

        UnionSet(List<V> values) {
            nodes = new HashMap<V, Node<V>>();
            parents = new HashMap<Node<V>, Node<V>>();
            sizeMap = new HashMap<Node<V>, Integer>();
            for (V value : values) {
                Node<V> node = new Node<V>(value);
                nodes.put(value, node);
                parents.put(node, node);
                sizeMap.put(node, 1);
            }
        }

        public Node<V> Find(Node<V> node) {
            Stack<Node<V>> stack = new Stack<Node<V>>(); // 用于路径压缩,存储沿途的节点
            while (node != parents.get(node)) { // 节点没有指向自己不是根节点,继续
                node = parents.get(node);
                stack.push(node);
            }

            while (!stack.isEmpty()) { // 进行路径压缩,在存在大量节点可以看到效果
                parents.put(stack.pop(), node);
            }

            return node;
        }

        public void Union(V node1, V node2) {
            Node<V> parent1 = Find(nodes.get(node1));
            Node<V> parent2 = Find(nodes.get(node2));
            if (parent1 != parent2) {  // 两者不属于一个集合
                Integer size1 = sizeMap.get(parent1);
                Integer size2 = sizeMap.get(parent2);
                Node<V> biggerNode = (size1 >= size2) ? parent1 : parent2;
                Node<V> smallNode = (biggerNode == parent1) ? parent2 : parent1;
                parents.put(smallNode, biggerNode);  // 将小树挂到大树上
                sizeMap.put(biggerNode, size1 + size2);
                sizeMap.remove(smallNode);
            }
        }

        public boolean connected(V node1, V node2) {
            return Find(nodes.get(node1)) == Find(nodes.get(node2));
        }

        public Integer size() {
            return sizeMap.size();
        }
    }
}

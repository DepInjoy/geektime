package algorithm.training.struct.AC;

import java.util.LinkedList;
import java.util.Queue;

/**
 * @brief:      解决在一个大字符串中，找到多个候选字符串的问题
 *
 *              实现思路：
 *                  1. 将所有的字符串形成一颗前缀树(insert方法)
 *                  2. 前缀树节点增加fail指针(build方法)，其中，fail指针指向
 *                      其他的候选字符串中和当前形成路径拥有最大的匹配长度的符串的最后一个字符所对应的节点
 * */
public class D32_04_AC1 {
    public static class Node {
        // 有多少字符串以该节点结尾
        public int end;
        public Node fail;
        public Node[] nexts;

        Node() {
            end = 0;
            fail = null;
            nexts = new Node[26];
        }
    }

    public static class ACAutomation {
        // 根节点
        private Node root;

        public ACAutomation() {
            root = new Node();
        }

        // 构造前缀树
        public void insert(String s) {
            char[] str = s.toCharArray();
            Node cur = root;
            for (int i = 0; i < str.length; ++i) {
                // 确定属于哪一路
                int index = str[i] - 'a';
                if (cur.nexts[index] == null) {
                    Node node = new Node();
                    cur.nexts[index] = node;
                }
                cur = cur.nexts[index];
            }
            cur.end++;
        }

        // 为前缀树构造fail指针
        public void build() {
            Queue<Node> queue = new LinkedList<>();
            queue.add(root);
            Node cur = null;
            Node cfail = null;
            while (!queue.isEmpty()) {
                cur = queue.poll();
                for (int i = 0; i < 26;++i) {  // 下级所有的路
                    if (cur.nexts[i] != null) { // 该路下存在子节点
                        // 先设置一个初始值
                        cur.nexts[i].fail = root;
                        cfail = cur.fail;
                        while (cfail != null) { // cur不是头节点
                            if (cfail.nexts[i] != null) {
                                cur.nexts[i].fail = cfail.nexts[i];
                                break;
                            }
                            cfail = cfail.fail;
                        }
                        queue.add(cur.nexts[i]);
                    }
                }
            }
        }

        public int containNum(String content) {
            char[] str = content.toCharArray();
            Node cur = root;
            Node follow = null;
            int index = 0;
            int ans = 0;
            for (int i = 0; i < str.length; ++i) {
                index = str[i] - 'a';
                while (cur.nexts[index] == null && cur != root) {
                    cur = cur.fail;
                }
                cur = cur.nexts[index] == null ? root : cur.nexts[index];

                follow = cur;
                while (follow != root) {
                    if (follow.end == -1) break;

                    {
                        // 针对不同的需求修改这里
                        ans += follow.end;
                        follow.end = -1;
                    }
                    follow = follow.fail;
                }
            }
            return ans;
        }

        public static void main(String[] args) {
            ACAutomation ac = new ACAutomation();
            ac.insert("dhe");
            ac.insert("he");
            ac.insert("c");
            ac.build();
            System.out.println(ac.containNum("cdhe"));
        }
    }
}

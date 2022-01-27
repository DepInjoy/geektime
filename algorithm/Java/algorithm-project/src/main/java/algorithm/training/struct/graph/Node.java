package algorithm.training.struct.graph;

import java.util.ArrayList;

// 节点结构描述
public class Node {
    public int value;
    // 入度
    public int in;
    // 出度
    public int out;
    // 节点指向的节点列表
    public ArrayList<Node>  nexts;
    public ArrayList<Edge>  edges;

    public Node(int value) {
        this.value = value;
        this.in = 0;
        this.out = 0;
        this.nexts = new ArrayList<Node>();
        this.edges = new ArrayList<Edge>();
    }

}

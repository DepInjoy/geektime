package algorithm.training.struct.trie;

/**
 * @brief:          前缀树Trie
 *
 *                  实现思想:
 *                      1. 单个字符串中，字符从前到后的加到一棵多叉树上
 *                      2. 字符放在路上，节点上有专属的数据项（常见的是pass和end值）
 *                      3. 所有样本都这样添加，如果没有路就新建，如有路就复用
 *                      4. 沿途节点的pass值增加1，每个字符串结束时来到的节点end值增加1
 *
 *                  相关接口：
 *                      1. void insert(String str) 添加某个字符串，可以重复添加，每次算1个
 *                      2. int search(String str) 查询某个字符串在结构中还有几个
 *                      3. void delete(String str) 删掉某个字符串，可以重复删除，每次算1个
 *                      4. int prefixNumber(String str) 查询有多少个字符串，是以str做前缀的
 *
 *                  实现方式：
 *                      数组或Hash表
 * */
public class C08_02_TrieTree {
}

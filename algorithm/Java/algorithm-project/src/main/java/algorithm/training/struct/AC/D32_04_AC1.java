package algorithm.training.struct.AC;

/**
 * @brief:      解决在一个大字符串中，找到多个候选字符串的问题
 *
 *              实现思路：
 *                  1. 将所有的字符串形成一颗前缀树(insert方法)
 *                  2. 前缀树节点增加fail指针(build方法)，其中，fail指针指向
 *                      其他的候选字符串中和当前形成路径拥有最大的匹配长度的符串的最后一个字符所对应的节点
 * */
public class D32_04_AC1 {
}

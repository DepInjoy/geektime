package algorithm.training.struct.Heap;

/**
 * @brief:      比较器，实质就是重载比较运算符
 *                  1. 比较器可以很好的应用在特殊标准的排序上
 *                  2. 比较器可以很好的应用在根据特殊标准排序的结构上
 *                  3. 写代码变得异常容易，还用于范型编程
 *
 *              比较器的统一约定：
 *                  @Override
 *                  public int compare(T o1, T o2)
 *                      返回负数的情况，就是o1比o2优先的情况
 *                      返回正数的情况，就是o2比o1优先的情况
 *                      返回0的情况，就是o1与o2同样优先的情况
 * */
public class C06_01_Comparator {
}

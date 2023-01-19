# 架构
## Memo

## Search&Job scheduler
扫描Memo并计算得到代价最小的执行计划，其实现工作可以划分为三步：
- Exploration,搜索和补全计划空间，就是根据优化规则不断生成语义相同的逻辑表达式。

    例如`select * from a, b where a.c1 = b.c2`,可以得到`a inner join b`和`b inner join a`。
- Implementation,实例化逻辑表达式为物理算子。
例如`a join b`可以实例化为`a hash_join b`和`a merge_join b`.

- 优化，添加计划的必要条件，之后对不同的执行计划计算最终的预估代价。

比如, sortGroupBy需要input是排序过的, 这是需要enforce order的property, 添加了该属性，ORCA在优化中会要求子节点都可以满足该要求，例如自己点满足 sort order property, 可以通过对其排序或节点本身就可以满足，类index scan。



# 参考资料
- [InfoQ:数据库内核杂谈(九)：开源优化器 ORCA](https://www.infoq.cn/article/5o16eHOZ5zk6FzPSJpT2)
- 
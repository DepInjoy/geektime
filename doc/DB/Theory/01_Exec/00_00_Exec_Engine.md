# 火山模型
在火山模型中，每个Operator，提供`Open() - Next() - Close()`来进行生命周期管理:

- Open: 用于算子的初始化操作，一般也会调用子节点的该方法来初始化整棵树。
- Next: 根据算子类型进行具体的实现，首先会调用子节点的`Next()`方法，获取子节点的数据，进行特定的处理后(该算子的具体实现)，向上返回给父节点。
- Close: 关闭算子的生命周期。


# 向量模型
数据库向量化一般指基于CPU的向量化，因此数据库性能优化的本之在于：基于CPU的程序如何进行性能优化。这引出2个关键问题：
1. 如何衡量CPU性能？
2. 哪些因素影响CPU性能？

CPU Time = (instruction numbers) * CPI * (clock cycle time)。其中，instructions numbers指CPU产生的指令数；CPI(cycles per instruction)指执行一个指令需要的CPU周期；Clock cycle time是一个CPU周期需要的实际时间，和CPU硬件特性强相关。软件可以改变前两项。


# Pipeline模型
How do you make operators parallelism-aware?
How do we partition Sources?

# 参考资料
1. [向量化引擎怎么提升数据库性能](https://cloud.tencent.com/developer/article/2355179)
2. [数据库计算向量化](https://plantegg.github.io/2021/11/26/%E6%95%B0%E6%8D%AE%E5%BA%93%E8%AE%A1%E7%AE%97%E5%90%91%E9%87%8F%E5%8C%96/)

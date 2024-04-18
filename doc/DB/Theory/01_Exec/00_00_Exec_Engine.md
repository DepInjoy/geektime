# 火山模型
在火山模型中，每个Operator，提供`Open() - Next() - Close()`来进行生命周期管理:

- Open: 用于算子的初始化操作，一般也会调用子节点的该方法来初始化整棵树。
- Next: 根据算子类型进行具体的实现，首先会调用子节点的`Next()`方法，获取子节点的数据，进行特定的处理后(该算子的具体实现)，向上返回给父节点。
- Close: 关闭算子的生命周期。


# 向量模型

# Pipeline模型
How do you make operators parallelism-aware?
How do we partition Sources?

# 参考资料

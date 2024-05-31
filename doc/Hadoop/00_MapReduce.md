# MRV1
第一代MapReduce。它由两部分组成：
- 编程模型
- 运行时环境（计算框架）
设计的目的：
- 主要解决搜索引擎面临的海量数据处理扩展性差的问题
- 易于编程，简化分布式程序设计，用户只需专注于自己的应用程序逻辑实现

## MR编程模型

### 数据切分(Split)

作用：
1. 按照某个策略将数据切分成若干个split，确定Map Task个数
2. 给定某个split，能将其解析成一个个key/value对。

怎么切分？文件切分算法
Map任务启动在哪里？任务本地性？Host选择算法

## MR计算框架
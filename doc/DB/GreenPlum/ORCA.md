# ORCA架构
> 在 ORCA 构建的伊始，就制定了如下这些目标：
> 1. 模块化：开发的第一天起，ORCA 就完全以一个独立的模块进行开发。所有的数据输入和输出都接口化。这也是为了能够让 ORCA 能够兼容不同的数据库产品。
> 2. 可验证和测试性:在构建ORCA的同时，为了ORCA的可验证性和可测试性，同时构建了一批测试和验证工具，确保ORCA一直在正确的道路上迭代。
> 3. 高延展性：算子类，优化规则(transformation rule)，数据类型类都支持扩展，使得 ORCA 可以不断迭代。方便更高效地加入新的优化规则。
> 4. 高并发优化：ORCA内部实现了可以利用多核并发的调度器来进一步提高对复杂语句的优化效率。


## DB和ORCA交互
![](./img/gp-orca-interact.png)

DXL(Data eXchange Language)是ORCA暴露出的接口: DXL定制了一套基于XML语言的数据交互接口。这些数据包括：用户输入的查询语句，优化器输出的执行计划，数据库的元数据及数据分布等。

数据库输入 DXL Query 和 DXL Metadata，然后 ORCA 返回 DXL plan。任何数据库系统只要实现 DXL 接口，理论上都可以使用 ORCA 进行查询优化。DXL 接口的另一个好处就在于，大幅度简化了优化器的测试，验证 bug 修复的难度。只需要通过 DXL 输入 mock(假数据)数据，就可以让 ORCA 进行优化并输出执行结果，而不需要真正搭建一个实体数据库来操作。

## 架构机制
![ORCA架构图](./img/pg-orca-arch.png)
ORCA架构分成以下几个模块：
### Memo

### Search&Job scheduler

### Plan Transformation

### Property Enforcement

### Metadata Cache

### GPOS



# 参考资料
[数据库内核杂谈（九）：开源优化器ORCA](https://www.infoq.cn/article/5o16ehoz5zk6fzpsjpt2)
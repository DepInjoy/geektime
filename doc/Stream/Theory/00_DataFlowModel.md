---

<b><font color="orange">论文想要解决什么问题？</font></b>

---

# DataFlow Model

<b><font color="orange">定义系统模型并解释其语义可以满足含标准批处理、微批处理和流模型以及Lambda结构的混合流处理和批处理语义</font></b>

## 核心原语

首先，考虑经典批处理模型中的原语。对流经系统的`(key, value)`，DataFlow SDK有两个核心的转换(transform)：

- `ParDo`用于通用并行处理。每个待处理的输入元素(其本身可能是一个有限集合)都会被用户自定义函数(在DataFlow中称为`DoFn`)处理，该函数可以为每个输入产生零或多个输出。例如，考虑这样的操作，展开输入key的所有前缀，并复制value。
  $$
  \begin{array}{l}
  (fix, 1), (fit, 2) \\
  \ \ \ \ \ \ \downarrow Pardo(ExpandP ref ixes) \\
  (f,1), (f_i,1), (fix, 1), (f, 2), (fi, 2), (fit, 2)
  \end{array}
  $$

- `GroupByKey`用于`(key, value)`分组
  $$
  \begin{array}{l}
  (f,1), (fi,1), (fix, 1), (f, 2), (fi, 2), (fit, 2) \\
  \ \ \ \ \ \ \downarrow GroupByKey \\
  (f, [1,2]), (fi, [1,2]), (fix, [1]), (fit, [2])
  \end{array}
  $$
  `ParDo`操作在每个输入元素做个操作，因而可以很自然应用到无边界数据。`GroupByKey`操作收集指定key的所有数据，在将它们发送到下游缩减前。如果输入源数据是无界的，便无法知道合何时结束。该问题的常见解决方案是将数据窗口化。



## 窗口化

支持分组的系统通常将`GroupByKey`重新定义为`GroupByKeyAndWindow`。本文的主要贡献是支持未对齐的窗口，对此主要有两点。其一，从模型角度，将所有的窗口当做未对齐窗口，且允许底层实现在适用情况下对对齐情况的优化，更加简单；其二，窗口可以分解成两个相关操作：

- `Set<Window> AssignWindows(T datum)`，将元素赋值给零或多个窗口。
- `Set<Window> MergeWindows(Set<Window> windows)`，按照分组时间进行窗口合并。这允许数据驱动的窗口在数据到达并分组在一起时，随着时间的推移而构建。

对于任何给定的窗口策略，这两个操作都密切相关，如sliding window assign需要sliding window merging，session window assignment需要ssession window merging，等等。

注意，为了原生支持事件时间的窗口，传递`(key, value, eventtime, window)` 4元组，而f非`(key, value)`到系统。带事件时间的元素传递给系统，并在最初时分配一个默认的全局窗口，覆盖所有事件时间，提供与标准批处理模型中默认值匹配的语义。



### 窗口分配(Window Assignment)

> From the model’s perspective, window assignment creates a new copy of the element in each of the windows to which  it has been assigned.

从模型的的角度来看，窗口分配是在每个已分配的窗口中，创建元素的新副本。例如，2分钟时间宽度1分钟为周期的滑动窗口：
$$
\begin{array}{l}
(k, v1, 12:00, [0, \infty )),(k, v2, 12:01, [0, \infty )) \\
\ \ \ \ \ \ \ \downarrow AssignW indows(Sliding(2m, 1m)) \\
(k, v1, 12:00, [11:59, 12:01)), \\
(k, v1, 12:00, [12:00, 12:02)),\\
(k, v2, 12:01, [12:00, 12:02)), \\
(k, v2, 12:01, [12:01, 12:03))\\
\end{array}
$$
在本例中，这两个`(key, value)`都被复制到重叠元素时间戳的两个窗口中。由于窗口直接与所属的元素相关联，这意味着窗口分配可以发生在分组前，管道(Pipeline)中的任何位置。这一点很重要，因为分组操作可能隐藏在下游复合(Composite)转换的任何地方，例如`Sum.integersPerKey()`。

### 窗口合并(Window Merging)



然后，开始`GroupByKeyAndWindow`操作，实际上，这由五部分组成的复合操作：

1. `DropTimestamps`：丢弃元素时间戳，因为从这里开始，只有窗口相关的部分。
2. `GroupByKey`：
3. `MergerWindow`
4. `GroupAlsoByWindow`
5. `ExpandToElements`

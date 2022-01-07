# 二叉树

## 二叉树的遍历

二叉树的遍历，核心问题是二维结构线性化。

- 从根节点访问其左右儿子节点
- 访问左儿子，右儿子节点怎么办？
    - 那么，我们需要一个存储结构来暂存不访问的节点，这些存储结构便是堆栈、队列

二叉树的遍历主要有四种方式

- 前序遍历，遍历的过程:
```
1. 访问根节点
2. 先序遍历其左子树
3. 先序遍历其右子树
```

- 中序遍历，遍历的过程:
```
1. 中序遍历左子树
2. 访问根节点
3. 中序遍历右子树
```

- 后序遍历，遍历过程:
```
1. 后续遍历左子树
2. 后序遍历右子树
3. 访问根节点
```

- 层序遍历，遍历过程

```
1. 根节点入队
2. 节点出队，访问节点,左右儿子入队
```

上述四种遍历方式，大家常常谈及的是前序遍历、中序遍历和后序遍历，它们适合用栈来实现。这三种遍历方式对于每个节点都会被访问三次，这个访问次序也就是**递归序**(参考递归实现，进栈出栈)，假如，有下面的一棵树

``` 
								1
							---  ---
						   /		\
						   2         3
						 -- --     --  --
						/     \   /      \
					   4       5 6        7
```

对应的递归序是：

``` 
		   1 2 4 4 4 2 5 5 5 2 1 3 6 6 6  3 7 7 7 3 1
先序遍历:	 1 2 4       5         3 6       7
中序遍历:	       4   2   5     1     6    3  7
后序遍历：			 4       5 2         6       7 3 1
```



三者的差别是第几次达到节点的时候执行对节点的操作：

- 先序遍历是第一次到达节点执行操作
- 中序遍历是第二次到达节点执行操作
- 后序遍历是第三次到达节点执行操作

常用递归的方式来实现，也可以借助栈采用非递归的方式来实现，这一点浙大在MOOC上的讲解很清晰，另外[掘金:二叉树非递归遍历](https://juejin.cn/post/6844903503807119374)采取的实现方式和浙大思想很相近，同时这里面也有非递归的后序遍历的实现，相对来说这一版的实现个人觉得，模板性更强，推荐。

层次遍历使用用队列实现，从尾部进队列，从头部出。

看了一些资料实现了几版二叉树的遍历实现，记录几个个人比较喜欢的版本

- [二叉树的前中后序递归遍历](https://github.com/DepInjoy/geektime/blob/main/algorithm/Java/algorithm-project/src/main/java/algorithm/training/struct/BT/C10_02_RecursiveTraversalBT.java)
- [二叉树的前中后序非递归遍历](https://github.com/DepInjoy/geektime/blob/main/algorithm/Java/algorithm-project/src/main/java/algorithm/training/struct/BT/UnrecursiveTraversalBT_2_Star.java)
- [二叉树的层次遍历](https://github.com/DepInjoy/geektime/blob/main/algorithm/Java/algorithm-project/src/main/java/algorithm/training/struct/BT/C11_01_LevelTraversalBT.java)


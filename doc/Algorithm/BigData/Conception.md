# 近似算法
近似算法主要用来解决优化问题，能够给出一个优化问题的近似优化解的算法

近似算法解的近似度
- 问题的每一个可能的解都具有一个代价
- 问题的优化解可能具有最大或最小代价
- 我们希望寻找问题的一个误差最小的近似优化解

## 分析近似解代价与优化解代价的差距

### Ratio Bound
$$
\begin{array}{l}
设A是一个优化问题的近似算法, A具有ratio \ bound \ p(n), 如果\\
max(\frac{C}{C^*}, \frac{C*}{C}) \le p(n) \\
其中n是输入大小, C是A产生的解的代价, C^*是优化解的代价. \\
\\
如果问题是最大化问题, max(\frac{C}{C^*}, \frac{C*}{C})=\frac{C^*}{C} \\
如果问题是最小化问题, max(\frac{C}{C^*}, \frac{C*}{C})=\frac{C}{C^*} \\
由于\frac{C}{C^*} < 1当且仅当\frac{C^*}{C} > 1, 因此Ratio \ Bound永远不会小于1\\
Ratio Bound越大, 近似解越坏
\end{array}
$$
### 相对误差
- 相对误差: 对于任意输入, 近似算法的相对误差定义为
$$
\begin{array}{c}
\frac{|C-C^*|}{C^*} \\
\\
其中:C是近似解的代价, C^*是优化解的代价. 
\end{array}
$$

- 相对误差界: 一个近似算法的相对误差界为$\varepsilon(n)$, 如果
$$
\begin{array}{c}
\frac{|C-C^*|}{C^*} \le \varepsilon(n) \\
\\
其中:C是近似解的代价, C^*是优化解的代价. 
\end{array}
$$
### $(1+\varepsilon)$-近似
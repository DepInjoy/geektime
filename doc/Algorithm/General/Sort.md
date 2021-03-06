# 排序算法
此部分是结合浙江大学的MOOC陈越、何钦铭的《数据结构》为主线进行排序算法实践和记录。
## 内部排序
```
void X_Sort ( ElementType A[], int N )
	大多数情况下，为简单起见，讨论从小大的整数排序
	N是正整数
	只讨论基于比较的排序(> = < 有定义)
	只讨论内部排序，内部排序是指待排序列完全存放在内存中所进行的排序过程，适合不太大的元素序列。

稳定性：任意两个相等的数据，排序前后的相对位置不发生改变
```
|排序算法|稳定性|平均时间复杂度|最好时间复杂度|最坏时间复杂度|空间复杂度
|-------|------|---------|-------|--------|--|
|冒泡排序|稳定|$O(N^2)$|O(N)，数据已经有序|$O(N^2)$数据恰好逆序|O(1)|
|插入排序|稳定|$O(N^2)$|O(N)，数据已经有序|$O(N^2)$数据恰好逆序|O(1)|
|快排|不稳定|$\Theta (NlogN)$|$\Omega(NlogN)$每次都正好中分|$O(N^2)$|$O(logN)$|
|归并排序|稳定|$\Theta (NlogN)$|$\Omega(NlogN)$|$O(NlogN)$|O(N)|
|堆排序|稳定|$\Theta(NlogN)$|$\Omega(NlogN)$|$O(NlogN)$|O(1)|
### 简单排序

#### 冒泡排序
稳定性:稳定
时间复杂度：
- 最坏情况:`O(N^2)`，当输入的数据是逆序，每一趟冒泡都需要数据交换并将其中一个数据放在应该存放的位置，数据复杂度为`O(N(N-1))`即`O(N^2)`
- 最好情况:`O(N)`,当输入正好是按照顺序已排好的数据时，只需要遍历一遍，无数据交换，即可。

#### 插入排序
稳定性:稳定
时间复杂度:
- 最坏情况:`O(N^2)`，当输入的数据是逆序
- 最好情况:`O(N)`，输入的数据正好是已经排好的数据，遍历一遍就好

> 时间复杂度下界
> - 对于下标i<j，如果A[i]>A[j]，则称(i,j)是一对逆序对(inversion)
>     - 交换2个相邻元素正好消去1个逆序对！
>     - 插入排序：T(N, I)=O( N+I ) — 如果序列基本有序，则插入排序简单且高效
>
> 定理：任意N个不同元素组成的序列平均具有N(N-1)/4个逆序对。
> 
> 定理：任何仅以交换相邻两元素来排序的算法，其平均时间复杂度为O(N^2)。

因此，要提高算法效率，我们必须
- 每次消去不止1个逆序对！
- 每次交换相隔较远的2个元素！

### 希尔排序
实质上是一种分组插入排序，也称递减增量排序算法，是插入排序的改进版，是一种不稳定的排序算法。
> 定义增量序列 D(M) > D(M-1) > … > D(1) = 1
> 对每个 Dk 进行“Dk-间隔”排序( k = M, M-1, … 1 )
> 注意：“Dk-间隔”有序的序列，在执行“Dk-1-间隔”排序后，仍然是“Dk-间隔”有序的
***
参考
- 1. [RUNOOB.COM:希尔排序](https://www.runoob.com/w3cnote/shell-sort.html)
- 2. [知乎:【算法】排序算法之希尔排序](https://zhuanlan.zhihu.com/p/122632213)

希尔排序是基于插入排序的以下两点性质而提出改进方法的[1]：
- 插入排序在对几乎已经排好序的数据操作时，效率高，即可以达到线性排序的效率；
- 但插入排序一般来说是低效的，因为插入排序每次只能将数据移动一位；

基本思想[2]：
先将整个待排元素序列分割成若干个子序列（由相隔某个“增量”的元素组成的）分别进行直接插入排序，然后依次缩减增量再进行排序，待整个序列中的元素基本有序（增量足够小）时，再对全体元素进行一次直接插入排序。

***
#### 原始希尔排序
最坏情况：`T=theta(N^2)`

坏例子:增量元素不互质，则小增量可能根本不起作用
#### 更多增量序列
##### Hibbard 增量序列
`D(k) = 2^k – 1` — 相邻元素互质
最坏情况: `T =N^(3/2)`
猜想：`Tavg = O(N^5/4)`

##### Sedgewick增量序列
{1, 5, 19, 41, 109, … } 
--- `9*4^i–9*2^i+1` 或 `4^i–3*2^i+1`
猜想：`Tavg=O(N^7/6)`，`Tworst =O(N^4/3)`

### 选择排序
`A[i]`到`A[N–1]`中找最小元,可以实现交换较远的元素，可能达到一次消去多个逆序对。
```C++
void Selection_Sort ( ElementType A[], int N ) {
	for ( i = 0; i < N; i ++ ) {
		MinPosition = ScanForMin( A, i, N–1 ); 
		/* 从A[i]到A[N–1]中找最小元，并将其位置赋给MinPosition */
		Swap( A[i], A[MinPosition] );
		/* 将未排序部分的最小元换到有序部分的最后位置 */
	}
}
```
需要交换的次数为`O(N)`，无论如何`T=theta(N^2)`,想要优化问题便转化为如何寻找最小元？由此堆排序上场

### 堆排序
堆排序是对选择排序的一种改进。

### 归并排序


### 快速排序
快速排序也是一种分而治之的思想，选定一个主元，将数据划分为两个子集，实现将小于主元的数据放在左边，将大于主元的数据放在右边，同时将主元放在其原本就应该在位置，之后再分别对左边和右边部分分别进行上述的快速排序。

最好情况：```T(N) = O(NlogN)，当每次都恰好中分```


对于不同的窗口，最朴素地，我们可以将其全部分成不同的 Window。实际执行时，每个 Window 都需要先做一次排序，代价不小。是否可能利用一次排序计算多个窗口函数呢？某些情况下，是可能的。例如：

```sql
		... 
	ROW_NUMBER() OVER (PARTITION BY dealer_id ORDER BY sales) AS rank,
    AVG(sales) OVER (PARTITION BY dealer_id) AS avgsales
    	...
```

虽然这 2 个窗口不完全一致，但是 `AVG(sales)` 不关心分区数据的顺序，完全可以复用 `ROW_NUMBER()` 的窗口。[Optimization of Analytic Window Functions](http://vldb.org/pvldb/vol5/p1244_yucao_vldb2012.pdf)论文提供了一种启发式的算法，能尽可能利用能够复用的机会。
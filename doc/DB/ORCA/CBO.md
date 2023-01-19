Join直方图计算
```plantuml
@startuml

@enduml
```
Join等值连接直方图计算
```C++
// construct new histogram by joining with another histogram, no normalization
CHistogram *
CHistogram::MakeJoinHistogram(CStatsPred::EStatsCmpType stats_cmp_type,
							  const CHistogram *histogram) const
```
```C++
// construct a new histogram with equality join
CHistogram *
CHistogram::MakeJoinHistogramEqualityFilter(const CHistogram *histogram) const
```

谓词过滤
```C++
// derive statistics for filter operation based on given scalar expression
IStatistics *
CFilterStatsProcessor::MakeStatsFilterForScalarExpr(
	CMemoryPool *mp, CExpressionHandle &exprhdl, IStatistics *child_stats,
	CExpression *local_scalar_expr,	 // filter expression on local columns only
	CExpression *
		outer_refs_scalar_expr,	 // filter expression involving outer references
	IStatisticsArray *all_outer_stats)
```

Group by统计信息计算
```C++
// Statistics helper routines for processing group by operations
// return statistics object after Group by computation
CStatistics *
CGroupByStatsProcessor::CalcGroupByStats(CMemoryPool *mp,
										 const CStatistics *input_stats,
										 ULongPtrArray *GCs,
										 ULongPtrArray *aggs, CBitSet *keys)
```
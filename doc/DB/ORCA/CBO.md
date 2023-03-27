# 统计信息
```C++
// Compute statistics for filter operation
class CFilterStatsProcessor {
public:
	// filter
	static CStatistics *MakeStatsFilter(CMemoryPool *mp,
			const CStatistics *input_stats,CStatsPred *base_pred_stats,
			BOOL do_cap_NDVs);

	// derive statistics for filter operation based on given scalar expression
	static IStatistics *MakeStatsFilterForScalarExpr(
		CMemoryPool *mp, CExpressionHandle &exprhdl, IStatistics *child_stats,
		CExpression*local_scalar_expr,	 // filter expression on local columns only
		CExpression * outer_refs_scalar_expr,	 // filter expression involving outer references
		IStatisticsArray *all_outer_stats);

	static CDouble SelectivityOfPredicate(CMemoryPool *mp, CExpression *pred,
		CTableDescriptor *ptabdesc, CColRefSet *pcrsOuterRefs);
};


class CInnerJoinStatsProcessor : public CJoinStatsProcessor {};
```
## 直方图
```C++
// estimate data skew by sampling histogram buckets,
// the estimate value is >= 1.0, where 1.0 indicates no skew
//
// skew is estimated by computing the second and third moments of
// sample distribution: for a sample of size n, where x_bar is sample mean,
// skew is estimated as (m_3/m_2^(1.5)), where m_2 = 1/n Sum ((x -x_bar)^2), and
// m_3 = 1/n Sum ((x -x_bar)^3)
//
// since we use skew as multiplicative factor in cost model, this function
// returns (1.0 + |skew estimate|)
void
CHistogram::ComputeSkew()
```

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
	CExpression *outer_refs_scalar_expr,	 // filter expression involving outer references
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

# 代价模型(Cost Model)
```C++
class CCostModelGPDB : public ICostModel {
private:
	// number of segments
	ULONG m_num_of_segments;

	// cost model parameters
	CCostModelParamsGPDB *m_cost_model_params;
};

class CCostModelParamsGPDB : public ICostModelParams {
private:
	// array of parameters
	// cost param enum is used as index in this array
	SCostParam *m_rgpcp[EcpSentinel];
};
```

```C++
class ICostModel : public CRefCount {
	class CCostingStats : public CRefCount {
	private:
		// stats of the root
		IStatistics *m_pstats;
	};

	struct SCostingInfo {
	private:
		// stats of the root
		CCostingStats *m_pcstats;
	}

public:
    // return number of hosts (nodes) that store data
 	virtual ULONG UlHosts() const = 0;

	// return number of rows per host
	virtual CDouble DRowsPerHost(CDouble dRowsTotal) const = 0;

	// return cost model parameters
	virtual ICostModelParams *GetCostModelParams() const = 0;

	// main driver for cost computation
	virtual CCost Cost(CExpressionHandle &exprhdl,
					   const SCostingInfo *pci) const = 0;

	// cost model type
	virtual ECostModelType Ecmt() const = 0;

	// set cost model params
	void SetParams(ICostModelParamsArray *pdrgpcp) const;

	// create a default cost model instance
	static ICostModel *PcmDefault(CMemoryPool *mp);
};
```

```C++
class ICostModelParams : public CRefCount {
public:
	struct SCostParam {
	private:
		// param identifier
		ULONG m_id;

		// param value
		CDouble m_value;

		// param lower bound
		CDouble m_lower_bound_val;

		// param upper bound
		CDouble m_upper_bound_val;
	};
}
```

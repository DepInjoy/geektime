# 查询优化
入口函数是`libgpopt\include\gpopt\optimizer\COptimizer.h`的`static CDXLNode *PdxlnOptimiz`。
```c++
	static CDXLNode *PdxlnOptimize(
		CMemoryPool *mp,
		CMDAccessor *md_accessor,  // MD accessor
		const CDXLNode *query,
		const CDXLNodeArray
			*query_output_dxlnode_array,  // required output columns
		const CDXLNodeArray *cte_producers,
		IConstExprEvaluator *pceeval,  // constant expression evaluator
		ULONG ulHosts,		// number of hosts (data nodes) in the system
		ULONG ulSessionId,	// session id used for logging and minidumps
		ULONG ulCmdId,		// command id used for logging and minidumps
		CSearchStageArray *search_stage_array,	// search strategy
		COptimizerConfig *optimizer_config,		// optimizer configurations
		const CHAR *szMinidumpFileName =
			NULL  // name of minidump file to be created
	);
```

# 任务调度
任务调度的入口函数在`libgpopt\src\search\CScheduler.cpp`的`CScheduler::FExecute`
```C++
BOOL
CScheduler::FExecute(CJob *pj, CSchedulerContext *psc)
```
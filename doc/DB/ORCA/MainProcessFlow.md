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
执行的主流程,从
```plantuml
@startuml
COptimizer -> CTranslatorDXLToExpr:PexprTranslateQuery
note right of CTranslatorDXLToExpr : translate DXL Tree -> Expr Tree
COptimizer -> CQueryContext:PqcGenerate
note right of CQueryContext : Generate the query context for the given \nexpression and array of output column ref ids
COptimizer -> COptimizer:**PexprOptimize**
note right of COptimizer : optimize logical expression tree into physical expression tree

COptimizer -> COptimizer: CreateDXLNode
note right of COptimizer : translate plan into DXL
@enduml
```
查询优化主要的调用流程在`COptimizer::PexprOptimize`，执行流程
```plantuml
@startuml
COptimizer -> CEngine:Init
== CEngine:Init ==
CEngine -> CSearchStage:PdrgpssDefault
note right of CSearchStage : Generate default search strategy
CEngine -> CEngine:InitLogicalExpression
note right of CEngine : Initialize engine with a given expression
group InitLogicalExpression
CEngine -> CEngine:PgroupInsert
CEngine -> CMemo:SetRoot
end
== CEngine:Optimize ==
COptimizer -> CEngine:Optimize
group Optimize
	CEngine -> CJobFactory:CJobFactory
	note left of CJobFactory: new CScheduler as jf
	CEngine -> CScheduler: CScheduler
	note left of CScheduler: new CScheduler as sched
	CEngine -> CSchedulerContext:Init
	note left of CSchedulerContext : init CSchedulerContext with CJobFactory & CScheduler
	loop ul ->[0 : ulSearchStages)
		CEngine -> COptimizationContext : new COptimizationContext as poc
		CEngine -> CEngine: **ScheduleMainJob**
		note right of CEngine: schedule main optimization job

		
		CEngine -> CScheduler:**Run()**
		note right of CScheduler : run optimization job
		CEngine ->Memo:**PexprExtractPlan**
		note left of Memo: extract best plan found at the end of current search stage
		CEngine -> CSearchStage:SetBestExpr
		note right of CSearchStage : Set best plan found at the end of search stage
		CEngine -> CEngine:FinalizeSearchStage
		note right of CEngine : Execute operations after search stage completes
	end
	alt CEnumeratorConfig::FSample()
		CEngine -> CEngine:SamplePlans
		note right of CEngine : Sample distribution of possible plans uniformly
	end
end
== CEngine:PexprExtractPlan ==

COptimizer -> CEngine:PexprExtractPlan
@enduml
```
- `CEngine::ScheduleMainJob`中创建了`CJobGroupOptimization`,并将其添加到`m_listjlWaiting`等待队列中。
```C++
class CSchedulerContext {
private:
	CScheduler *m_psched{nullptr};  // scheduler
		......
public:
	// CScheduler通过Add接口将将Job添加到m_listjlWaiting
	CScheduler * Psched() const{ return m_psched; }
		......
};

class CScheduler {
private:
  // list of jobs waiting to execute
  CSyncList<SJobLink> m_listjlWaiting;
  ......
};
```
- `CScheduler::Run()`从WaitingList中取出Job并执行
```plantuml
CScheduler -> CScheduler:Run
CScheduler -> CScheduler:ExecuteJobs
note right of CScheduler : Main job processing task

group CScheduler:ExecuteJobs
    loop nullptr != (pj = PjRetrieve())
    	CScheduler -> CScheduler:PreExecute
    	note right of CScheduler : prepare for job execution
    	
		CScheduler -> CScheduler:FExecute
    	note right of CScheduler : execute job
    	
		CScheduler -> CScheduler:EjrPostExecute
    	note right of CScheduler : process job result
    end
end
```
- `Memo::PexprExtractPlan`从当前的SearchStage中提取最优的计划。

# 任务调度
任务调度的入口函数在`libgpopt\src\search\CScheduler.cpp`的`CScheduler::FExecute`
```C++
BOOL
CScheduler::FExecute(CJob *pj, CSchedulerContext *psc)
```
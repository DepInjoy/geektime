```C++
class CJobGroup : public CJob;
class CJobGroupExploration : public CJobGroup;
class CJobGroupImplementation : public CJobGroup;
class CJobGroupOptimization : public CJobGroup;


class CJobGroupExpression : public CJob;
class CJobGroupExpressionExploration : public CJobGroupExpression;
class CJobGroupExpressionImplementation : public CJobGroupExpression;
class CJobGroupExpressionOptimization : public CJobGroupExpression;
```

```C++
// 每个job持有一个CJobQueue，CJobQueue头部的job称为owner是MainJob
// 其他的job是EjqrQueued
class CJob {
private:
    // parent job
	CJob *m_pjParent{nullptr};
	// assigned job queue
	CJobQueue *m_pjq{nullptr};
	// 引用计数, 当为1时可以恢复parent job，参见FResumeParent
	ULONG_PTR m_ulpRefs{0};

	// job id - set by job factory
	ULONG m_id{0};

	// job type
	EJobType m_ejt;

	// 标识Job是否已经初始化,在CJob::SetInit中设置
    // 负责具体职责的Job重写Init接口并调用CJob::SetInit
	BOOL m_fInit{false};
public:
    // 纯虚接口，所有的Job都需要实现该接口的实现
    // CScheduler通过该接口Excute Job,进而触发状态机的状态变换
	BOOL FExecute(CSchedulerContext *psc) override = 0;
    
    SLink m_linkQueue;
};
```



```C++
class CJobGroup : public CJob {
private:
protected:
	// target group
	CGroup *m_pgroup{nullptr};

	// last scheduled group expression
	CGroupExpression *m_pgexprLastScheduled;

	// ctor
	CJobGroup() = default;

	// dtor
	~CJobGroup() override = default;

	// initialize job
	void Init(CGroup *pgroup);

	// get first unscheduled logical expression
	virtual CGroupExpression *PgexprFirstUnschedLogical();

	// get first unscheduled non-logical expression
	virtual CGroupExpression *PgexprFirstUnschedNonLogical();

	// get first unscheduled expression
	virtual CGroupExpression *PgexprFirstUnsched() = 0;

	// schedule jobs for of all new group expressions
	virtual BOOL FScheduleGroupExpressions(CSchedulerContext *psc) = 0;


}


```

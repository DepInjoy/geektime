

> Orca uses a search mechanism to navigate through the space of possible plan alternatives and identify the plan with the least estimated cost.The search mechanism is enabled by a specialized Job Scheduler that creates dependent or parallel work units to perform query optimization in three main steps: exploration, where equivalent logical expressions are generated, implementation where physical plans are generated, and optimization, where required physical properties (e.g., sort order) are enforced and plan alternatives are costed.
>
> Orca使用一种搜索机制在可能的计划空间中导航，改变土著，并以最小的估计成本确定计划。搜索机制由专门的作业调度器启用，该调度器创建依赖的或并行的工作单元，以三个主要步骤执行查询优化:探索(生成等价的逻辑表达式)、实现(生成物理计划)和优化(执行所需的物理属性(例如排序顺序)并计算计划替代)。

在ORCA的入口`COptimizer::PexprOptimize`允许用户通过`CSearchStageArray *search_stage_array`输入Search策略。如果`search_stage_array`为空采用`CSearchStage::PdrgpssDefault`来生成默认的Search策略。在`CEngine::Init`接口调用中

```C++
//  Generate default search strategy;
//  one stage with all xforms and no time/cost thresholds
CSearchStageArray *
CSearchStage::PdrgpssDefault(CMemoryPool *mp) {
    CXformSet *xform_set = GPOS_NEW(mp) CXformSet(mp);
    // 所有支持的Exploration集合
    xform_set->Union(CXformFactory::Pxff()->PxfsExploration());
    CSearchStageArray *search_stage_array = GPOS_NEW(mp) CSearchStageArray(mp);
    search_stage_array->Append(GPOS_NEW(mp) CSearchStage(xform_set));

    return search_stage_array;
}
```

`CSearchStage`数据结构的表示:

```C++
class CSearchStage {
private:
    // set of xforms to be applied during stage
    CXformSet *m_xforms;

    // 以毫秒为单位的搜索时间阈值,默认为ulong_max
    ULONG m_time_threshold;
    // 运行时间和m_time_threshold一起决定Search是否超时
    // 参见FTimedOut()实现
    CTimerUser m_timer;

    // cost阈值，more为0.0
    CCost m_cost_threshold;

    // best plan found at the end of search stage
    CExpression *m_pexprBest;

    // cost of best plan found
    CCost m_costBest;

public:
    BOOL FTimedOut() const {
        if (m_time_threshold == gpos::ulong_max) {
            return false;
        }
        return m_timer.ElapsedMS() > m_time_threshold;
    }

    BOOL FAchievedReqdCost() const {
        return (nullptr != m_pexprBest && m_costBest <= m_cost_threshold);
    }
};
```

# Job调度主流程

Job调度通过CEngine:Optimize发起,去除一些不重要的代码了解调用流程

```C++
// Main driver of optimization engine
void CEngine::Optimize() {    
	const ULONG ulJobs = std::min((ULONG) GPOPT_JOBS_CAP,
            (ULONG)(m_pmemo->UlpGroups() * GPOPT_JOBS_PER_GROUP));
    CJobFactory jf(m_mp, ulJobs);
    CScheduler sched(m_mp, ulJobs);

    CSchedulerContext sc;
	// 1. 初始化SchedulerContext
	// 		1.1. 所有worker的内存池为m_mp
	// 		1.2. 为当前worker创建暂存内存池(内部私有的m_pmpLocal)
    sc.Init(m_mp, &jf, &sched, this);

    const ULONG ulSearchStages = m_search_stage_array->Size();
    for (ULONG ul = 0; !FSearchTerminated() && ul < ulSearchStages; ul++) {
        // 2. 重置当前search stage的Timer
        PssCurrent()->RestartTimer();

        // optimize root group
        COptimizationContext *poc = GPOS_NEW(m_mp) COptimizationContext(
            m_mp, PgroupRoot(), m_pqc->Prpp(),
            GPOS_NEW(m_mp) CReqdPropRelational(GPOS_NEW(m_mp) CColRefSet(m_mp)),
            GPOS_NEW(m_mp) IStatisticsArray(m_mp),
            m_ulCurrSearchStage);

        // 创建和schedule main optimization job
        ScheduleMainJob(&sc, poc);

        // run optimization job(job状态机开始状态切换)
        CScheduler::Run(&sc);

        poc->Release();

        // 在当前search stage的末尾提取目前找到的最优的plan
        CExpression *pexprPlan = m_pmemo->PexprExtractPlan(
            m_mp, m_pmemo->PgroupRoot(), m_pqc->Prpp(),
            m_search_stage_array->Size());
        // 为当前Search Stage设置最优计划
        PssCurrent()->SetBestExpr(pexprPlan);

        // 当前search stage完成, 执行一些操作
        //  1. 标识当前SearchStage的index加1,即m_ulCurrSearchStage++
        //  2. reset Memo Group中的state和jobqueue，参见CMemo::ResetGroupStates
        FinalizeSearchStage();
    }

    if (CEnumeratorConfig::FSample()) {
        SamplePlans();
    }
}
```



```C++
class CSchedulerContext {
private:
	// memory pool used by all workers
	CMemoryPool *m_pmpGlobal{nullptr};
	// memory pool used by only by current worker(scratch space暂存空间)
	CMemoryPool *m_pmpLocal{nullptr};

	// job工厂类可以根据job类型创建相应的job
	CJobFactory *m_pjf;
	// scheduler
    // job会通过一个Add接口添加进来，并push到内部m_listjlWaiting
	CScheduler *m_psched{nullptr};

	// optimization engine
	CEngine *m_peng;

	// 表示该context是否已经初始化(通过CSchedulerContext::Init初始化)
	BOOL m_fInit{false};
};

// Initialize scheduling context
void CSchedulerContext::Init(CMemoryPool *pmpGlobal, CJobFactory *pjf,
						CScheduler *psched, CEngine *peng) {
	// 当前worker的暂存空间
	m_pmpLocal = CMemoryPoolManager::GetMemoryPoolMgr()->CreateMemoryPool();
	m_pmpGlobal = pmpGlobal;
	m_pjf = pjf;
	m_psched = psched;
	m_peng = peng;
	m_fInit = true;
}
```



```C++
// 创建main optimization job
void CEngine::ScheduleMainJob(CSchedulerContext *psc,
        COptimizationContext *poc) const {
    CJobGroupOptimization::ScheduleJob(
        psc, PgroupRoot(), nullptr /*pgexprOrigin*/, poc, nullptr /*pjParent*/);
}

void CJobGroupOptimization::ScheduleJob(CSchedulerContext *psc, CGroup *pgroup,
           CGroupExpression *pgexprOrigin,
           COptimizationContext *poc, CJob *pjParent) {
	// 通过CJobFactory根据JobType创建job，此处创建CJobGroupOptimization
    CJob *pj = psc->Pjf()->PjCreate(CJob::EjtGroupOptimization);

	CJobGroupOptimization *pjgo = PjConvert(pj);
    // 初始化JobGroupOptimization
    //	  1. 初始化JobStateMachine,参见CJobStateMachine::Init
    // 	  2. 为JobStateMachine设置jobAction，参见CJobStateMachine::SetAction
    // 设置JobQueue
	pjgo->Init(pgroup, pgexprOrigin, poc);
    // 将pjgo添加到Scheduler中的waiting list(m_listjlWaiting)
    // 排队的job数量(m_ulpQueued)加1
	psc->Psched()->Add(pjgo, pjParent);
}
```



```C++
// Main job processing task
void *CScheduler::Run(void *pv) {
	CSchedulerContext *psc = reinterpret_cast<CSchedulerContext *>(pv);
    // 取出CSchedulerContext中的CScheduler调用ExecuteJobs
	psc->Psched()->ExecuteJobs(psc);
	return nullptr;
}

void CScheduler::ExecuteJobs(CSchedulerContext *psc) {
	CJob *pj = nullptr;
	ULONG count = 0;

    // 从m_listjlWaiting取出一个job进行处理
	while (nullptr != (pj = PjRetrieve())) {
		// prepare for job execution,实现
        //   1. 运行中的job数量(m_ulpRunning)加1
		PreExecute(pj);
		BOOL fCompleted = FExecute(pj, psc);
		// process job result
		switch (EjrPostExecute(pj, fCompleted)) {
			case EjrCompleted:
				// job is completed
				Complete(pj);
				psc->Pjf()->Release(pj);
				break;

			case EjrRunnable:
				// child jobs have completed, job can immediately resume
				Resume(pj);
				continue;

			case EjrSuspended:
				// job is suspended until child jobs complete
				Suspend(pj);
				break;

			default:
				GPOS_ASSERT(!"Invalid job execution result");
		}

		if (++count == OPT_SCHED_CFA) {
			GPOS_CHECK_ABORT;
			count = 0;
		}
	}
}
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
}
```



```C++
// Execution function using job queue
BOOL CScheduler::FExecute(CJob *pj, CSchedulerContext *psc) {
	BOOL fCompleted = true;
	CJobQueue *pjq = pj->Pjq();

	// check if job is associated to a job queue
	if (nullptr == pjq) {
		fCompleted = pj->FExecute(psc);
	} else {
		switch (pjq->EjqrAdd(pj)) {
			case CJobQueue::EjqrMain:
				// main job, runs job operation,取决于job的状态机
                 // 例如:CJobGroupOptimization::FExecute返回m_jsm.FRun(psc, this)
				fCompleted = pj->FExecute(psc);
				if (fCompleted) { // main job已经完成
					// m_listjQueued的头部的job移除，并恢复该job的parent job
					pjq->NotifyCompleted(psc);
				} else {
					// task is suspended
					(void) pj->UlpDecrRefs();
				}
				break;

			case CJobQueue::EjqrQueued:
				// queued job
				fCompleted = false;
				break;

			case CJobQueue::EjqrCompleted:
				break;
		}
	}
	return fCompleted;
}

```

```C++
// Job队列先进先出，等待job队列的头部Job成为MainJob(m_pj)
CJobQueue::EJobQueueResult CJobQueue::EjqrAdd(CJob *pj) {
	EJobQueueResult ejer = EjqrCompleted;
	// check if job has completed before getting the lock
	if (!m_fCompleted) {
		if (pj == m_pj) { // check if this is the main job
			ejer = EjqrMain;
		} else {
			// check if job is completed
			if (!m_fCompleted) {
				m_listjQueued.Append(pj);
                 // 等待job队列的头部Job称为MainJob
				BOOL fOwner = (pj == m_listjQueued.First());
				if (fOwner) { // first caller becomes the owner
					m_pj = pj;
					ejer = EjqrMain;
				} else {
					ejer = EjqrQueued;
				}
			}
		}
	}
	return ejer;
}
```



```C++
// Notify waiting jobs of job completion
void CJobQueue::NotifyCompleted(CSchedulerContext *psc) {
	m_fCompleted = true;
	while (!m_listjQueued.IsEmpty()) {
		CJob *pj = m_listjQueued.RemoveHead();
		// check if job execution has completed
		if (1 == pj->UlpDecrRefs()) {
			// update job as completed
			psc->Psched()->CompleteQueued(pj);
			// recycle job
			psc->Pjf()->Release(pj);
		}
	}
}

void CScheduler::CompleteQueued(CJob *pj) {
    // 重新恢复调度parent job
    // 将pj添加到m_listjlWaiting的头部进行调度
    // 参见CScheduler::Resume -> CScheduler::Schedule
	ResumeParent(pj);
	// update statistics
	m_ulpTotal--;
	m_ulpStatsCompleted++;
	m_ulpStatsCompletedQueued++;
}

void CScheduler::ResumeParent(CJob *pj) {
	CJob *pjParent = pj->PjParent();
	if (nullptr != pjParent) {
		// notify parent job
		if (pj->FResumeParent()) { 
			// reschedule parent
			Resume(pjParent);
			// update statistics
			m_ulpStatsResumed++;
		}
	}
}
```


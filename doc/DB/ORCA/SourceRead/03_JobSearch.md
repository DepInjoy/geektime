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

CSearchStage数据结构的表示:
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

Job调度通过CEngine:Optimize发起,去除一些不重要的代码了解调用流程
```C++
    const ULONG ulJobs = std::min((ULONG) GPOPT_JOBS_CAP,
            (ULONG)(m_pmemo->UlpGroups() * GPOPT_JOBS_PER_GROUP));
    CJobFactory jf(m_mp, ulJobs);
    CScheduler sched(m_mp, ulJobs);

    CSchedulerContext sc;
    sc.Init(m_mp, &jf, &sched, this);

    const ULONG ulSearchStages = m_search_stage_array->Size();
    for (ULONG ul = 0; !FSearchTerminated() && ul < ulSearchStages; ul++) {
        // 重置当前search stage的Timer
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
        extract best plan found at the end of current search stage
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

// Create and schedule the main optimization job
void CEngine::ScheduleMainJob(CSchedulerContext *psc,
        COptimizationContext *poc) const {
    CJobGroupOptimization::ScheduleJob(
        psc, PgroupRoot(), nullptr /*pgexprOrigin*/, poc, nullptr /*pjParent*/);
}
```


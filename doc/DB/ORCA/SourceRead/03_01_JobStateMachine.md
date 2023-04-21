`Job`执行的状态机采用`CJobStateMachine`表示，其内部拥有一组与状态对应的`Job Action`函数指针数组和表示`Job`状态的状态机(是一个`N*N`的二维数组和表示当前状态的枚举值表示)

```C++
template <class TEnumState, TEnumState estSentinel, class TEnumEvent,
		  TEnumEvent eevSentinel>
class CJobStateMachine {
private:
    // Job Action的函数指针
	using PFuncAction = TEnumEvent (*)(CSchedulerContext *, CJob *);
	using SM = CStateMachine<TEnumState, estSentinel, TEnumEvent, eevSentinel>;

	// 与状态对应的Job Action函数指针数组
	PFuncAction m_rgPfuncAction[estSentinel];
	SM m_sm; // Job状态机
};

template <class TEnumState, TEnumState tenumstateSentinel, class TEnumEvent,
		  TEnumEvent tenumeventSentinel>
class CStateMachine {
private:
	TEnumState m_tenumstate; // 当前状态
	BOOL m_fInit{false}; // 状态机是否进行初始化
	// 状态转移是一个N*N的二维数组
	TEnumEvent m_rgrgtenumeventTransitions[tenumstateSentinel][tenumstateSentinel];
};
```



其对外提供下面的操作：

```
初始化状态机
void Init(const TEnumEvent rgfTransitions[estSentinel][estSentinel]);

设置相应状态的Job Action函数指针
void SetAction(TEnumState est, PFuncAction pfAction)

运行状态机，返回值表示当前Job是否已经完成
BOOL FRun(CSchedulerContext *psc, CJob *pjOwner)

重置状态机
void Reset()
```



详细了解相关的实现

```C++
void Init(const TEnumEvent rgfTransitions[estSentinel][estSentinel])	{
    Reset();
    // 状态机初始化
    // 	 1. 将当前状态初始化为0
    //   2. 将输入的rgfTransitions复制给内部的m_rgrgtenumeventTransitions
    //   3. 将是否初始的标识(m_fInit)设置为true
    m_sm.Init(rgfTransitions);
}

void Reset() {
	m_sm.Reset();
	for (ULONG i = 0; i < estSentinel; i++) {
        m_rgPfuncAction[i] = nullptr;
	}
}
```



```C++
BOOL FRun(CSchedulerContext *psc, CJob *pjOwner) {
    TEnumState estCurrent = estSentinel;
    TEnumState estNext = estSentinel;
    do {
        // check if current search stage is timed-out
        if (psc->Peng()->PssCurrent()->FTimedOut()) {
            // 清理Job并终止当前Job(返回true,使得调度器认为当前Job完成调度下一个Job)
            pjOwner->Cleanup();
            return true;
        }

        // 获取当前状态, 根据当前状态获取Job Action函数并执行
        estCurrent = m_sm.Estate();
        PFuncAction pfunc = m_rgPfuncAction[estCurrent];
        TEnumEvent eev = pfunc(psc, pjOwner);

        // use the event to transition state machine
        estNext = estCurrent;
        // CStateMachine::FAttemptTransition完成真实的状态转换
        // eev代表stateOld， estNext代表stateNew
        // for each : eventTransitions[stateOld][j=0:N] == stateNew
        //     查找成功, stateNew = j
        BOOL fSucceeded GPOS_ASSERTS_ONLY = m_sm.FTransition(eev, estNext);
    } while (estNext != estCurrent && estNext != m_sm.TesFinal());
    return (estNext == m_sm.TesFinal());
}

// CStateMachine::FAttemptTransition
BOOL FAttemptTransition(TEnumState tenumstateOld, TEnumEvent tenumevent,
                   TEnumState &tenumstateNew) const {
    for (ULONG ulOuter = 0; ulOuter < tenumstateSentinel; ulOuter++) {
        if (m_rgrgtenumeventTransitions[tenumstateOld][ulOuter] == tenumevent) {
            tenumstateNew = (TEnumState) ulOuter;
            return true;
        }
    }
    return false;
}
```




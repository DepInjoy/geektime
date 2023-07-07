状态机是有限状态自动机的简称，有四大概念：

- State(状态)，一个状态机至少包含两个状态。
- Event(事件)，也叫Transition Condition(转移条件)，在某一状态下，达到了转移条件，按照状态机的转移流程转移到下一状态，并执行相应的动作，也就是执行某个操作的触发条件或者口令。
- Action(动作)，时间发生以后需要执行的动作，一般一个Action对应一个函数。
- Transition(转移)，也就是从一个状态变化为另一个状态。在一个状态接收输入执行了某些动作到达了另外一个状态的过程就是一个transition(转移)。定义transition(转移)就是在定义状态机的转移流程。

`Job`执行的状态机采用`CJobStateMachine`表示，这里实现状态的转移，并封装了`CJobStateMachine`在状态转移时执行相应的动作，并对外提供统一的接口。



`CStateMachine`的数据结构表示和对外提供的操作

```C++
template <class TEnumState, TEnumState tenumstateSentinel,
		 class TEnumEvent, TEnumEvent tenumeventSentinel>
class CStateMachine {
private:
	TEnumState m_tenumstate; // 当前状态
	BOOL m_fInit; // 是否初始化标志
	// 事件变换(transitions)数组
    // 表示满足转移条件(Event)时，从上一个状态转移(old_state)到下一个状态(new_state)
    // 		event = m_rgrgtenumeventTransitions[old_state][new_state]
	TEnumEvent m_rgrgtenumeventTransitions[tenumstateSentinel][tenumstateSentinel];
public:
    // 尝试转移transition,如果查找到满足转移条件的状态则返回true,否则返回false
	BOOL FTransition(TEnumEvent tenumevent, TEnumState &tenumstate);
    // 不关心是否满足转移条件
	void Transition(TEnumEvent tenumevent);
	// 获取当前状态
	TEnumState Estate() const;
     // 获取初始化状态和最终状态
	TEnumState TesInitial() const;
	TEnumState TesFinal() const;
	// reset状态机
	void Reset();
};
```

主要的操作实现是

```C++
// 状态机初始化 
void Init(const TEnumEvent rgrgtenumeventTransitions[tenumstateSentinel][tenumstateSentinel]) {
     // 1. 将输入的rgfTransitions复制给内部的m_rgrgtenumeventTransitions
    for (ULONG ulOuter = 0; ulOuter < tenumstateSentinel; ulOuter++) {
        for (ULONG ulInner = 0; ulInner < tenumstateSentinel; ulInner++) {
            m_rgrgtenumeventTransitions[ulOuter][ulInner] =
                rgrgtenumeventTransitions[ulOuter][ulInner];
        }
    }
     // 2. 将当前状态初始化为0
    m_tenumstate = TesInitial();
     // 3. 将是否初始的标识(m_fInit)设置为true
    m_fInit = true;
}

// 尝试转移transition
BOOL FTransition(TEnumEvent tenumevent, TEnumState &tenumstate) {
    TEnumState tenumstateNew;
    BOOL fSucceeded = FAttemptTransition(m_tenumstate, tenumevent, tenumstateNew);
    if (fSucceeded) {
        m_tenumstate = tenumstateNew;
    }
    tenumstate = m_tenumstate;
    return fSucceeded;
}

// 实际实现执行转换
BOOL FAttemptTransition(TEnumState tenumstateOld, TEnumEvent tenumevent,
                   TEnumState &tenumstateNew) const {
     // 遍历所有可能的状态,查找转移条件
    for (ULONG ulOuter = 0; ulOuter < tenumstateSentinel; ulOuter++) {
         // 从上一个状态(tenumstateOld)到下一个状态(ulOuter)满足转移条件(tenumevent)
         // 则查找到需要的状态，设置新状态(ulOuter)，并返回true
         // 不存在这样的状态，返回false，状态不转移
        if (m_rgrgtenumeventTransitions[tenumstateOld][ulOuter] == tenumevent) {
            tenumstateNew = (TEnumState) ulOuter;
            return true;
        }
    }
    return false;
}
```



`CJobStateMachine`的数据结构表示和对外提供的操作

```C++
template <class TEnumState, TEnumState estSentinel, class TEnumEvent,
		  TEnumEvent eevSentinel>
// 借助CStateMachine实现状态转移,这里在状态转移时执行相应动作(Action)
class CJobStateMachine {
private:
    // Job Action的函数指针
	using PFuncAction = TEnumEvent (*)(CSchedulerContext *, CJob *);
    // CStateMachine封装状态转移实现
	using SM = CStateMachine<TEnumState, estSentinel, TEnumEvent, eevSentinel>;

	// 与状态对应的Job Action函数指针数组
	PFuncAction m_rgPfuncAction[estSentinel];
	SM m_sm; // Job状态转移状态机
public:
    // 初始化状态机
	void Init(const TEnumEvent rgfTransitions[estSentinel][estSentinel]);
    // 设置相应状态的Job Action函数指针
    void SetAction(TEnumState est, PFuncAction pfAction)

    // 运行状态机，返回值表示当前Job是否已经完成
    BOOL FRun(CSchedulerContext *psc, CJob *pjOwner)

    // 重置状态机
    void Reset()
};
```



下面了解一下相关操作的实现,主要的实现是`FRun`来了解相关实现

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

        // 获取当前状态, 根据当前状态获取Action函数并执行
        estCurrent = m_sm.Estate();
        PFuncAction pfunc = m_rgPfuncAction[estCurrent];
        // 执行Action,获取转移条件(event)
        TEnumEvent eev = pfunc(psc, pjOwner);
        // 使用event来转移状态
        estNext = estCurrent;
 
        // 尝试转移转移, 如果满足状态转移条件(event)则获取到下一个状态(estnext)
        // 并返回true,否则false且下一个状态依然是当前状态(estNext=estCurrent)
        BOOL fSucceeded GPOS_ASSERTS_ONLY = m_sm.FTransition(eev, estNext);
    } while (estNext != estCurrent && estNext != m_sm.TesFinal());
    return (estNext == m_sm.TesFinal());
}
```



# 参考资料

- [掘金:什么是状态机？一篇文章就够了](https://juejin.cn/post/7118021175065722893)


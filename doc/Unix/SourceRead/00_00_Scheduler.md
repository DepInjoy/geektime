`kernel/sched/core.c`的
```C
/**
 *  内核调度接口,这里是一个对外的封装
 *  由__schedule接口完成调度的主体任务
*/
asmlinkage __visible void __sched schedule(void) {
    struct task_struct *tsk = current;
    sched_submit_work(tsk);
    do {
        preempt_disable();
        // 调度实现主函数
        __schedule(SM_NONE);
        sched_preempt_enable_no_resched();
    } while (need_resched());
    sched_update_worker(tsk);
}
EXPORT_SYMBOL(schedule);
```

```C
// 
static void __sched notrace __schedule(unsigned int sched_mode) {
            ......
    next = pick_next_task(rq, prev, &rf);
            ......
}

static struct task_struct *
pick_next_task(struct rq *rq, struct task_struct *prev, struct rq_flags *rf) {
	return __pick_next_task(rq, prev, rf);
}
```


```C
static inline struct task_struct *
__pick_next_task(struct rq *rq, struct task_struct *prev, struct rq_flags *rf) {
                    ...
    // kernel/sched/fair.c
    p = pick_next_task_fair(rq, prev, rf)
                    ...
}
```

```plantuml
@startuml
core -> core:__schedule
core -> core:pick_next_task
@enduml
```
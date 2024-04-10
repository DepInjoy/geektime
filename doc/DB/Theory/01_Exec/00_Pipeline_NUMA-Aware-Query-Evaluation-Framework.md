论文讲述的是adaptive morsel-driven query execution framework

**Morsel-driven query execution** is a new parallel query evaluation framework that fundamentally differs from the traditional Volcano model in that it  <b><font color=FA8072>distributes work between threads dynamically using work-stealing</font></b>. This prevents unused CPU resources due to load imbalances, and allows for elasticity, i.e., CPU resources can be reassigned between different queries at any time.

- A set of fast **parallel algorithms** for the most important relational operators.
- A systematic approach to integrating **NUMA-awareness** into database systems.



An important part of the morsel-wise framework is awareness of data locality.  -- elastic parallel processing

morsel-wise scheduling is flexible, but strongly favors scheduling choices that maximize NUMA-local execution.

---

背景和改进实现

<b><font color=FA8072>morsel-driven</font></b> query execution framework, where scheduling becomes a <b><font color=FA8072>fine-grained run-time task</font></b> that is NUMA-aware. Morsel-driven query processing takes small fragments of input data (“morsels”) and schedules these to worker threads that run entire operator pipelines until the next pipeline breaker. 



Such implementations of the Volcano model can be called <b><font color=FA8072>plan-driven</font></b>: the optimizer statically determines at <b><font color=FA8072>query compile time </font></b>how many threads should run, instantiates one query operator plan for each thread, and connects these with exchange operators.

---

核心思想：

The core idea is a scheduling mechanism (the “dispatcher”) that allows flexible parallel execution of an operator pipeline, that can change the parallelism degree even during query execution. 



Our dispatcher runs a fixed, machine-dependent number of threads, such that even if new queries arrive there is no resource over-subscription, and these threads are pinned to the cores, such that no unexpected loss of NUMA locality can occur due to the OS moving a thread to a different core.



The crucial feature of morsel-driven scheduling is that <b><font color=FA8072>task distribution is done at run-time and is thus fully elastic</font></b>. This allows to achieve perfect load balancing, even in the face of uncertain size distributions of intermediate results, as well as the hard-to-predict performance of modern CPU cores that varies even if the amount of work they get is the same. It is elastic in the sense that it can <b><font color=FA8072>handle workloads that change at run-time (by reducing or increasing the parallelism of already executing queries in-flight) and can easily integrate a mechanism to run queries at different priorities.</font></b>

---

The morsel-driven execution of the algebraic plan is controlled by a so called QEPobject which transfers executable pipelines to a dispatcher. It is the QEPobject’s responsibility to observe data dependencies. 

For each pipeline the QEPobject allocates the temporary storage areas into which the parallel threads executing the pipeline write their results.  After completion of the entire pipeline the temporary storage areas are logically re-fragmented into equally sized morsels.


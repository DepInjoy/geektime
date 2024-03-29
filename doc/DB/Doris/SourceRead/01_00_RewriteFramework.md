rewite的调用入口来自`NereidsPlanner`的`rewrite`
```java
public class NereidsPlanner extends Planner {
                ......
    private void rewrite() {
        Rewriter.getWholeTreeRewriter(cascadesContext).execute();
        NereidsTracer.logImportantTime("EndRewritePlan");
    }
                ......
}
```
`Rewriter`继承`AbstractBatchJobExecutor`，`Rewriter`提供一系列RewriteJob并实现`getJobs()`接口，其父类`AbstractBatchJobExecutor`实现`execute()`实现Job执行采用统一逻辑。

```java
public class Rewriter extends AbstractBatchJobExecutor {
    private final List<RewriteJob> rewriteJobs;

            .....
    @Override
    public List<RewriteJob> getJobs() {
        return rewriteJobs;
    }
            ......
}

// Base class for executing all jobs.
// Each batch of rules will be uniformly executed.
public abstract class AbstractBatchJobExecutor {
    protected CascadesContext cascadesContext;

    public void execute() {
            ......
    }

    public abstract List<RewriteJob> getJobs();
}
```

# Job执行

```plantuml
@startuml
abstract class AbstractBatchJobExecutor {
    # CascadesContext cascadesContext
    + abstract List<RewriteJob> getJobs();

    + void execute()

    + static RewriteJob bottomUp(List<RuleFactory> ruleFactories)
    + static RewriteJob topDown(List<RuleFactory> ruleFactories)

    + static TopicRewriteJob topic(String topicName,\n\tRewriteJob... jobs)
    + static RewriteJob custom(RuleType ruleType,\n\tSupplier<CustomRewriter> planRewriter)
    + static RewriteJob costBased(RewriteJob... jobs)
}

class Rewriter {
    + List<RewriteJob> getJobs()
}

class Analyzer {
    + List<RewriteJob> getJobs()
    + void analyze()
}
interface RewriteJob {
    + void execute(JobContext jobContext);
    + boolean isOnce();
}

class CascadesContext {
    - final RuleSet ruleSet;
    - final JobPool jobPool;
    - final JobScheduler jobScheduler;
    - JobContext currentJobContext;
    + void pushJob(Job job)
}
note top of CascadesContext : 上层的接口是其子类Rewriter或Analyzer\n通过它们放入构造函数将memo context传递给下来

interface ScheduleContext {
    + JobPool getJobPool();
    + void pushJob(Job job);
}

class TopicRewriteJob {
    + final String topicName;
    + final List<RewriteJob> jobs;
}
note bottom : 不可直接执行，一系列的RewriteJob组成\n在构造时将它们收集到jobs列表

class CustomRewriteJob {
    - RuleType ruleType;
    - final Supplier<CustomRewriter>\n\tcustomRewriter;
}
note bottom:自定义重写,需提供CustomRewriter实现

class CostBasedRewriteJob{}
note bottom: Cost based rewrite job

class RootPlanTreeRewriteJob {}
note bottom: 需要提供RewriteJobBuilder函数式编程接口\n封装实现bottomUp，topDown
Rewriter -down-|> AbstractBatchJobExecutor
Analyzer -down-|> AbstractBatchJobExecutor

CascadesContext -left-o AbstractBatchJobExecutor

CascadesContext -down.|> ScheduleContext
RewriteJob -up-* AbstractBatchJobExecutor
TopicRewriteJob -up..|> RewriteJob
CustomRewriteJob -up..|> RewriteJob

CostBasedRewriteJob -up..|> RewriteJob
RootPlanTreeRewriteJob -up..|> RewriteJob
@enduml
```

```java
// Base class for executing all jobs.
// Each batch of rules will be uniformly executed.
public abstract class AbstractBatchJobExecutor {
    // 也就是CascadesContext, 在NereidsPlanner
    // 将CascadesContext传递给Rewriter以及Optimizer
    protected CascadesContext cascadesContext;
    // 获取需要执行的一系列Job，子类负责完成实现
    public abstract List<RewriteJob> getJobs();
    
    // 将待处理规则(表示为RewriteJob)并对外提供execute接口来执行任务
    public void execute() {
        for (int i = 0; i < getJobs().size(); i++) {
            JobContext jobContext = cascadesContext.getCurrentJobContext();
            RewriteJob currentJob = getJobs().get(i);
            if (currentJob instanceof CostBasedRewriteJob) {
                List<RewriteJob> remainJobs = getJobs()
                        .subList(i + 1, getJobs().size()).stream()
                        .filter(j -> !(j instanceof CostBasedRewriteJob))
                        .collect(Collectors.toList());
                jobContext.setRemainJobs(remainJobs);
            }
            do {
                jobContext.setRewritten(false);
                currentJob.execute(jobContext);
            } while (!currentJob.isOnce() && jobContext.isRewritten());
        }
    }
}
```

# Rewrite Job
```plantuml
@startuml
interface RewriteJob {
    void execute(JobContext jobContext);
    boolean isOnce();
}

class JobContext {
    # final PhysicalProperties requiredProperties
    # final ScheduleContext scheduleContext
    # List<RewriteJob> remainJobs
}
note bottom: scheduleContext也就是CascadesContext, 在NereidsPlanner\n将CascadesContext传递给Rewriter以及Optimizer

class Rule {
    + List<Plan> transform(Plan node, CascadesContext context)
}

JobContext -left--o RewriteJob
TopicRewriteJob -down..|> RewriteJob
CustomRewriteJob -down..|> RewriteJob
CostBasedRewriteJob -down..|> RewriteJob
RootPlanTreeRewriteJob -down..|> RewriteJob

RewriteJob -down..> Rule
@enduml
```
将Rewrite Job抽象为RewriteJob,在其上派生出TopicRewriteJob，CustomRewriteJob等，进行不同类型的Job执行
```java
public interface RewriteJob {
    void execute(JobContext jobContext);
    boolean isOnce();
}

public class TopicRewriteJob implements RewriteJob { ...... }
public class CustomRewriteJob implements RewriteJob { ...... }
public class CostBasedRewriteJob implements RewriteJob {......}
public class RootPlanTreeRewriteJob implements RewriteJob {.....}
```
在AbstractBatchJobExecutor提供一系列对外接口
```java
public abstract class AbstractBatchJobExecutor {
            ......
    public static TopicRewriteJob topic(String topicName, RewriteJob... jobs) {
        return new TopicRewriteJob(topicName, Arrays.asList(jobs));
    }

    public static RewriteJob costBased(RewriteJob... jobs) {
        return new CostBasedRewriteJob(Arrays.asList(jobs));
    }

    public static RewriteJob bottomUp(RuleFactory... ruleFactories) {
        return bottomUp(Arrays.asList(ruleFactories));
    }

    public static RewriteJob topDown(RuleFactory... ruleFactories) {
        return topDown(Arrays.asList(ruleFactories));
    }


    public static RewriteJob custom(RuleType ruleType, Supplier<CustomRewriter> planRewriter) {
        return new CustomRewriteJob(planRewriter, ruleType);
    }
}
```

## TopicRewriteJob
```java
// 不可直接执行，一系列的RewriteJob组成
// 在构造时将它们收集到jobs列表
public class TopicRewriteJob implements RewriteJob {
    public final String topicName;
    public final List<RewriteJob> jobs;

                ....
    @Override
    public void execute(JobContext jobContext) {
        throw new AnalysisException("should not execute topic rewrite job "
            + topicName + " directly.");
    }
}
```

## Custom Rewrite

Custom实际上就是调用指定的函数接口。
```plantuml

class CustomRewriteJob {
    void execute(JobContext context)
}

interface CustomRewriter {
    + Plan rewriteRoot(Plan plan, JobContext jobContext);
}

interface RewriteJob {
    + void execute(JobContext jobContext);
    + boolean isOnce();
}
CustomRewriteJob .|> RewriteJob
CustomRewriter -up-* CustomRewriteJob
```

```java
public class CustomRewriteJob implements RewriteJob {
    private final RuleType ruleType;
    private final Supplier<CustomRewriter> customRewriter;

    public CustomRewriteJob(Supplier<CustomRewriter> rewriter, RuleType ruleType) {
            .....
    }

    @Override
    public void execute(JobContext context) {
        Set<String> disableRules = Job.getDisableRules(context);
        if (disableRules.contains(ruleType.name().toUpperCase(Locale.ROOT))) {
            return;
        }
        Plan root = context.getCascadesContext().getRewritePlan();
        Plan rewrittenRoot = customRewriter.get().rewriteRoot(root, context);
        context.getCascadesContext().setRewritePlan(rewrittenRoot);
    }

    @Override
    public boolean isOnce() {
        return false;
    }
}

// 函数式编程接口
public interface CustomRewriter {
    Plan rewriteRoot(Plan plan, JobContext jobContext);
}
```


## CostBasedRewrite
```java
public class CostBasedRewriteJob implements RewriteJob {

}
```
## RootPlanTreeRewrite
`AbstractBatchJobExecutor`对外提供`topDown`实现自上而下rewrite，也就是先rewrite自己，再rewrite孩子。
```java
public static RewriteJob topDown(RuleFactory... ruleFactories) {
    return topDown(Arrays.asList(ruleFactories));
}

public static RewriteJob topDown(List<RuleFactory> ruleFactories) {
    return topDown(ruleFactories, true);
}

public static RewriteJob topDown(List<RuleFactory> ruleFactories, boolean once) {
    List<Rule> rules = ruleFactories.stream()
            .map(RuleFactory::buildRules)
            .flatMap(List::stream)
            .collect(ImmutableList.toImmutableList());
    return new RootPlanTreeRewriteJob(rules, PlanTreeRewriteTopDownJob::new, once);
}
```

`AbstractBatchJobExecutor`对外提供`bottomUp`实现自下而上的rewrite，也就是写rewrite孩子，再rewrite自己。
```java
public static RewriteJob bottomUp(RuleFactory... ruleFactories) {
    return bottomUp(Arrays.asList(ruleFactories));
}

public static RewriteJob bottomUp(List<RuleFactory> ruleFactories) {
    List<Rule> rules = ruleFactories.stream()
            .map(RuleFactory::buildRules)
            .flatMap(List::stream)
            .collect(ImmutableList.toImmutableList());
    return new RootPlanTreeRewriteJob(rules, PlanTreeRewriteBottomUpJob::new, true);
}
```

在具体的实现过程中，借助`RootPlanTreeRewriteJob`将两个Job统一封装，提供`RewriteJobBuilder`函数式编程接口，借助方法引用`PlanTreeRewriteTopDownJob`和`PlanTreeRewriteBottomUpJob`分别实现Top-Down和Bottom-UP。
```plantuml
@startuml
class RootPlanTreeRewriteJob {
    - final List<Rule> rules;
    - final RewriteJobBuilder rewriteJobBuilder;
    - final boolean once;
}

interface RewriteJob {
    + void execute(JobContext jobContext);
    + boolean isOnce();
}
note bottom : rewrite job抽象接口

class RootRewriteJobContext {
    - final JobContext jobContext;
}

class RewriteJobContext {
    final boolean childrenVisited;
    final RewriteJobContext parentContext;
    final int childIndexInParentContext;
    final Plan plan;
    final RewriteJobContext[] childrenContext;
    Plan result;
}

interface RewriteJobBuilder {
    Job build(RewriteJobContext rewriteJobContext,
        \tJobContext jobContext, List<Rule> rules);
}
note bottom : 函数式编程接口，方法引用PlanTreeRewriteTopDownJob和\nPlanTreeRewriteBottomUpJob

RootRewriteJobContext -down-|> RewriteJobContext
RootPlanTreeRewriteJob -down.|> RewriteJob
RewriteJobBuilder -left-* RootPlanTreeRewriteJob

RewriteJobBuilder -right..> RootRewriteJobContext

@enduml
```

借助`RootPlanTreeRewriteJob`
```java
public class RootPlanTreeRewriteJob implements RewriteJob {
    private final List<Rule> rules;
    private final RewriteJobBuilder rewriteJobBuilder;
    private final boolean once;

    public RootPlanTreeRewriteJob(List<Rule> rules,
                RewriteJobBuilder rewriteJobBuilder, boolean once) {
        this.rules = Objects.requireNonNull(rules, "rules cannot be null");
        // 函数式编程接口，在AbstractBatchJobExecutor方法式引用
        // PlanTreeRewriteTopDownJob或PlanTreeRewriteBottomUpJob
        this.rewriteJobBuilder = Objects.requireNonNull(
                rewriteJobBuilder, "rewriteJobBuilder cannot be null");
        this.once = once;
    }

    @Override
    public void execute(JobContext context) {
        CascadesContext cascadesContext = context.getCascadesContext();
        Plan root = cascadesContext.getRewritePlan();
        RootRewriteJobContext rewriteJobContext = new RootRewriteJobContext(root, false, context);
        // 1. 引用PlanTreeRewriteTopDownJob和PlanTreeRewriteBottomUpJob
        Job rewriteJob = rewriteJobBuilder.build(rewriteJobContext, context, rules);
        // 2. rewriteJob加入job列表
        context.getScheduleContext().pushJob(rewriteJob);
        // 3. 取出Job执行
        cascadesContext.getJobScheduler().executeJobPool(cascadesContext);
    }

    // 函数式编程接口
    public interface RewriteJobBuilder {
        Job build(RewriteJobContext rewriteJobContext, JobContext jobContext, List<Rule> rules);
    }
}
```
### PlanTreeRewriteJob
接下来，了解负责Top-Down和Bottom-UP rewrite处理的`PlanTreeRewriteTopDownJob`和`PlanTreeRewriteBottomUpJob`的实现
```plantuml
@startuml
abstract class Job {
    # JobType type
    # JobContext context
    # boolean once
    # Set<String> disableRules
    + List<Rule> getValidRules(List<Rule> candidateRules)
    + abstract void execute()
}

abstract class PlanTreeRewriteJob {
    # RewriteResult rewrite(Plan plan, List<Rule> rules,\n\tRewriteJobContext rewriteJobContext)
}
note right : 统一实现改写\napply valid rule

class PlanTreeRewriteTopDownJob {
    - final RewriteJobContext rewriteJobContext;
    - final List<Rule> rules;
    + public void execute()
}
note top: 自上而下rewrite

class PlanTreeRewriteBottomUpJob {
    - final RewriteJobContext rewriteJobContext;
    - final List<Rule> rules;
    + public void execute()
}
note top: 自下而上rewrite

PlanTreeRewriteTopDownJob -down.|> PlanTreeRewriteJob
PlanTreeRewriteBottomUpJob -down.|> PlanTreeRewriteJob
PlanTreeRewriteJob -down.|> Job
@enduml
```

`PlanTreeRewriteJob`实现了`rewrite`接口,通过`Job`获取valid规则(Rule)，调用`transform`接口进行变换。
```java
public abstract class Job implements TracerSupplier {
    protected JobType type;
    protected JobContext context;
    protected boolean once;
    protected final Set<String> disableRules;

    // 实现统一获取valid规则接口实现
    // Doris支持会话级参数disable_nereids_rules禁用一系列Rule
    // JobContext有CascadesContext获取到会话级参数(SessionVariable)
    // 将输入中候选Rule去除禁用Rule得到valid Rule
    public List<Rule> getValidRules(List<Rule> candidateRules) {
            ......
    }
}

public abstract class PlanTreeRewriteJob extends Job {
    private final RewriteJobContext rewriteJobContext;
    private final List<Rule> rules;

    protected RewriteResult rewrite(Plan plan, List<Rule> rules,
            RewriteJobContext rewriteJobContext) {
                        .......
        List<Rule> validRules = getValidRules(rules);
        for (Rule rule : validRules) {
            Pattern<Plan> pattern = (Pattern<Plan>) rule.getPattern();
            if (pattern.matchPlanTree(plan)) {
                List<Plan> newPlans = rule.transform(plan, cascadesContext);
            }
            ......
        }
    }
}
```

#### Bottom-Up Rewrite
`PlanTreeRewriteBottomUpJob`也是一种Job，其主要的实现是`execute`接口。执行过程主要生Job然后状态切换，涉及下面几个状态
```java
enum RewriteState {
    // 可以处理当前节点，因为是自下而上处理，这表示所有的孩子已完成重写
    REWRITE_THIS,
    // 表示当前节点已处理完成
    REWRITTEN,
    // 表示处理当前节点生成了新计划，需要对其孩子节点重新处理
    ENSURE_CHILDREN_REWRITTEN
}
```
`execute`接口实现主要过程
```java
@Override
public void execute() {
    // 1. 由于是Bottom-up rewrite job，遍历并重置孩子状态为REWRITE_THIS
    boolean clearStatePhase = !rewriteJobContext.childrenVisited;
    if (clearStatePhase) {
        traverseClearState();
        return;
    }

    Plan plan = rewriteJobContext.plan;
    RewriteState state = getState(plan);
    switch (state) {
        case REWRITE_THIS:
            // 2. 调用rewrite进行重写，如果没有生成新Plan，状态切到REWRITTEN
            //    否则,切到ENSURE_CHILDREN_REWRITTEN,对其孩子重新处理
            rewriteThis();
            return;
        case ENSURE_CHILDREN_REWRITTEN:
            // 3.1 Plan被修改，重新处理孩子
            ensureChildrenRewritten();
            return;
        case REWRITTEN:
            // 3.2 Plan未被修改
            rewriteJobContext.result = plan;
            return;
    }
}
```

#### TopDown
`PlanTreeRewriteTopDownJob`也是一种Job，其主要的实现是`execute`接口。
```java
public void execute() {
    if (!rewriteJobContext.childrenVisited) {
        // 1. 自上而下, 先rewrite当前节点
        RewriteResult rewriteResult = rewrite(rewriteJobContext.plan, rules, rewriteJobContext);
        if (rewriteResult.hasNewPlan) {
            // 1.1 Plan被修改，生成新的Rewrite Job等下一次Job执行来执行Rewrite
            RewriteJobContext newContext = rewriteJobContext
                    .withPlanAndChildrenVisited(rewriteResult.plan, false);
            pushJob(new PlanTreeRewriteTopDownJob(newContext, context, rules));
            return;
        }
        
        // 2. 生成一个childrenVisited=true的Job，待孩子孩子处理完成，实现将孩子链接到当前节点
        //    参见下方的else处理,这样依赖Job Schedule是stack，先进后出
        //    下方处理孩子节点的Rewrite Job后进先出，这样可以确保将处理完的孩子链接到当前已处理的节点
        RewriteJobContext newRewriteJobContext = rewriteJobContext.withChildrenVisited(true);
        pushJob(new PlanTreeRewriteTopDownJob(newRewriteJobContext, context, rules));

        // 3. 生成RewriteJob处理孩子节点
        List<Plan> children = newRewriteJobContext.plan.children();
        for (int i = children.size() - 1; i >= 0; i--) {
            RewriteJobContext childRewriteJobContext = new RewriteJobContext(
                    children.get(i), newRewriteJobContext, i, false);
            if (!(rewriteJobContext.plan instanceof LogicalCTEAnchor)) {
                pushJob(new PlanTreeRewriteTopDownJob(childRewriteJobContext, context, rules));
            }
        }
    } else {
        // 2.1 所有孩子阶段已处理完成,将孩子连接到当前节点即可
        Plan result = linkChildrenAndParent(rewriteJobContext.plan, rewriteJobContext);
        if (rewriteJobContext.parentContext == null) {
            context.getCascadesContext().setRewritePlan(result);
        }
    }
}
```


# Optimize

## Derive Stat
```java
public class DeriveStatsJob extends Job {

}
```

## DPHyp
在`Optimizer`的`execute`可以看到`dpHypOptimize`的调用入口
```java
public void execute() {
            ....
    // DPHyp optimize
    int maxJoinCount = cascadesContext.getMemo().countMaxContinuousJoin();
    cascadesContext.getStatementContext().setMaxContinuousJoin(maxJoinCount);
    boolean isDpHyp = getSessionVariable().enableDPHypOptimizer
            || maxJoinCount > getSessionVariable().getMaxTableCountUseCascadesJoinReorder();
    cascadesContext.getStatementContext().setDpHyp(isDpHyp);
    cascadesContext.getStatementContext().setOtherJoinReorder(false);
    if (!getSessionVariable().isDisableJoinReorder() && isDpHyp
            && maxJoinCount <= getSessionVariable().getMaxJoinNumberOfReorder()) {
        //RightNow, dphyper can only order 64 join operators
        dpHypOptimize();
    }
            ......
}
```
可见，使用DPHyp和下面几个参数相关
```sql
SET enable_dphyp_optimizer = true;
max_table_count_use_cascades_join_reorder

set disable_join_reorder = true
max_join_number_of_reorder
```



Optimizer的`dpHypOptimize()`是DPHyp的入口
```java
private void dpHypOptimize() {
    Group root = cascadesContext.getMemo().getRoot();
    cascadesContext.pushJob(new JoinOrderJob(root, cascadesContext.getCurrentJobContext()));
    cascadesContext.getJobScheduler().executeJobPool(cascadesContext);
    // after DPHyp just keep logical expression
    cascadesContext.getMemo().removePhysicalExpression();
    cascadesContext.getStatementContext().setOtherJoinReorder(true);
}
```

```plantuml
@startuml
class JoinOrderJob {
    - final Group group;
    - final Set<NamedExpression> otherProject
    + void execute() throws AnalysisException
}

abstract class Job {
    + abstract void execute();
}
JoinOrderJob -down-|> Job
@enduml
```



## OptimizeGroupJob
```plantuml
@startuml
class CostAndEnforcerJob {

}
note top : Job to compute cost and add enforcer

abstract class Job {
    + abstract void execute();
}

CostAndEnforcerJob -down-|> Job
CostAndEnforcerJob -down.|> Cloneable

@enduml
```
```java
public class OptimizeGroupJob extends Job {
    private final Group group;
}
```
```
OptimizeGroupExpressionJob
CostAndEnforcerJob
```
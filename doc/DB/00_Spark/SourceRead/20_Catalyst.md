
Catalyst执行主流程
```scala
class QueryExecution(...) {
  protected def planner = sparkSession.sessionState.planner

  // 1. analyzer阶段,与catalog绑定生成LogicalPlan
  lazy val analyzed: LogicalPlan = {
    val plan = executePhase(QueryPlanningTracker.ANALYSIS) {
      sparkSession.sessionState.analyzer.executeAndCheck(logical, tracker)
    }
    plan
  }

  // 2. optimizer阶段，对LogicalPlan优化，生成Optimized LogicalPlan
  lazy val optimizedPlan: LogicalPlan = {
    executePhase(QueryPlanningTracker.OPTIMIZATION) {
      val plan = sparkSession.sessionState.optimizer.executeAndTrack(
          withCachedData.clone(), tracker)
      plan.setAnalyzed()
      plan
    }
  }

  // 3. SparkPlan阶段,Optimized LogicalPlan转换成PhysicalPlan
  lazy val sparkPlan: SparkPlan = {
    executePhase(QueryPlanningTracker.PLANNING) {
      QueryExecution.createSparkPlan(sparkSession, planner, optimizedPlan.clone())
    }
  }

  // 4. prepareForExecution阶段,将PhysicalPlan转换成Executed PhysicalPlan
  lazy val executedPlan: SparkPlan = {
    val plan = executePhase(QueryPlanningTracker.PLANNING) {
      QueryExecution.prepareForExecution(preparations, sparkPlan.clone())
    }
    tracker.setReadyForExecution()
    plan
  }

  // 5. execute阶段执行物理计划，得到RDD
  lazy val toRdd: RDD[InternalRow] = new SQLExecutionRDD(
    executedPlan.execute(), sparkSession.sessionState.conf)
}
```

```scala
abstract class LogicalPlan extends QueryPlan[LogicalPlan]
  with AnalysisHelper with LogicalPlanStats
  with LogicalPlanDistinctKeys with QueryPlanConstraints
  with Logging

// A logical plan node with single child.
trait UnaryNode extends LogicalPlan with UnaryLike[LogicalPlan]

// A logical plan node with no children.
trait LeafNode extends LogicalPlan with LeafLike[LogicalPlan]

// A logical plan node with a left and right child.
trait BinaryNode extends LogicalPlan with BinaryLike[LogicalPlan]
```
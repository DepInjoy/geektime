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
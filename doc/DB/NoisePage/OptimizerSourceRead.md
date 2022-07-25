查询优化的入口在`src\optimizer\optimizer.cpp`的`Optimizer::BuildPlanTree`接口
```C++
  /**
   * Build the plan tree for query execution
   * @param txn TransactionContext
   * @param accessor CatalogAccessor for catalog
   * @param storage StatsStorage
   * @param query_info Information about the query
   * @param op_tree Logical operator tree for execution
   * @param parameters parameters for the query, can be nullptr if there are no parameters
   * @returns execution plan
   */
  std::unique_ptr<OptimizeResult> BuildPlanTree(
      transaction::TransactionContext *txn, catalog::CatalogAccessor *accessor, StatsStorage *storage,
      QueryInfo query_info, std::unique_ptr<AbstractOptimizerNode> op_tree,
      common::ManagedPointer<std::vector<parser::ConstantValueExpression>> parameters) override;
```

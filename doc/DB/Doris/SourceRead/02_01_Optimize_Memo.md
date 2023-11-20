完成`rewrite`后，在`optimize`时，即`Optimizer::execute()`进行Memo的初始化
```java
    public void execute() {
        // 1. init memo
        cascadesContext.toMemo();
            ......
    }
}
```

构造Memo时调用init接口初始化Memo。
```java
public Memo(Plan plan) {
        root = init(plan);
}
```
```plantuml
@startuml
class Memo {
    - Map<GroupId, Group> groups
    - Group root
    - Group init(Plan plan)
    + Group getRoot()
    + void setRoot(Group root)
}

class Group {
    - GroupId groupId
    - List<GroupExpression> logicalExpressions
    - List<GroupExpression> physicalExpressions
    - List<GroupExpression> enforcers
    - LogicalProperties logicalProperties
    - Map<PhysicalProperties, Pair<Cost, GroupExpression>>
    - boolean isExplored = false
    - Statistics statistics
    + GroupExpression getLogicalExpression()
}

class GroupExpression {
    - Cost cost
    - Group ownerGroup
    - List<Group> children
    - Plan plan
    - BitSet ruleMasks
    - boolean statDerived
    - double estOutputRowCount = -1
    - Rule fromRule
}

class Statistics {
    - double rowCount
    - Map<Expression, ColumnStatistic> expressionToColumnStats
    - double tupleSize
}

Group -down-o Memo
GroupExpression -down-o Group
Statistics -left-o Group
@enduml
```


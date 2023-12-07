完成`rewrite`后，在`optimize`时，即`Optimizer::execute()`进行Memo的初始化
```java
    public void execute() {
        // 1. init memo
        cascadesContext.toMemo();
            ......
    }
}
```

# 逻辑算子
```plantuml
@startuml
abstract class AbstractTreeNode {
    - List<NODE_TYPE> children
    + List<NODE_TYPE> children()
    + NODE_TYPE child(int index)
}

abstract class AbstractPlan {

}

abstract class AbstractLogicalPlan {

}

interface Plan {
    + List<Slot> computeOutput()
}

abstract class LogicalUnary {

}
note right : for all logical plan\nthat have one child

abstract class LogicalLeaf {

}
note left : for all logical plan\nthat have no child

abstract class LogicalRelation {

}

interface BinaryPlan {

}

interface BinaryNode {
    + LEFT_CHILD_TYPE left();
    + RIGHT_CHILD_TYPE right()
    + default int arity()
}



abstract class LogicalBinary {
    + abstract List<Slot> computeOutput()
}

note right : for all logical plan\nthat have two children

AbstractPlan -down-|> AbstractTreeNode
AbstractLogicalPlan -down-|> AbstractPlan
LogicalUnary -down-|> AbstractLogicalPlan
LogicalLeaf -down-|> AbstractLogicalPlan

LogicalBinary -down-|> AbstractLogicalPlan
AbstractLogicalPlan -down.|> Plan

LogicalBinary -down.|> BinaryPlan
BinaryPlan -down.|> BinaryNode

LogicalJoin -down-|> LogicalBinary

LogicalFilter -down-|> LogicalUnary

LogicalRelation -down-|> LogicalLeaf
@enduml
```
# Memo
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
note left: Group中由等价的GroupExpression组成\n它们的统计信息相同，Statistics存储在Group中

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

Group -up-o Memo
GroupExpression -up-o Group
Statistics -up-o Group
@enduml
```
构造Memo调用`Memo::init`进行初始化
```plantuml
@startuml
class GroupPlan {
    - Group group
}
note right : A virtual node that represents\na sequence plan in a Group

abstract class AbstractLogicalPlan {}
abstract class LogicalLeaf {}

interface Plan {
}

interface TreeNode {
    + List<NODE_TYPE> children();
    + NODE_TYPE child(int index);
}

GroupPlan -down-|> LogicalLeaf
LogicalLeaf .|> AbstractLogicalPlan
AbstractLogicalPlan -down.|> Plan
Plan .|> TreeNode
@enduml
```
```java
private Group init(Plan plan) {
    // 1. 递归第对其孩子初始化
    initialize children recursively
    List<Group> childrenGroups = new ArrayList<>(plan.arity());
    for (Plan child : plan.children()) {
        childrenGroups.add(init(child));
    }

    plan = replaceChildrenToGroupPlan(plan, childrenGroups);
    // 2. plan + 其子节点Plan表示的Group, 创建GroupExpression
    GroupExpression newGroupExpression = new GroupExpression(plan, childrenGroups);
    // 3. 创建Group
    Group group = new Group(groupIdGenerator.getNextId(), newGroupExpression, plan.getLogicalProperties());

    // 4. 将创建的Group放在Memo的groups HashMap中
    groups.put(group.getGroupId(), group);
    groupExpressions.put(newGroupExpression, newGroupExpression);
    return group;
}

private Plan replaceChildrenToGroupPlan(Plan plan, List<Group> childrenGroups) {
    if (childrenGroups.isEmpty()) {
        return plan;
    }
    List<Plan> groupPlanChildren = childrenGroups.stream()
            .map(GroupPlan::new)
            .collect(ImmutableList.toImmutableList());
    return plan.withChildren(groupPlanChildren);
}
```

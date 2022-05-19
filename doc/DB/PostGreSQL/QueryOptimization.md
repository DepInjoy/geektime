# 查询优化简介

通常数据库查询优化方法分两个层次：

- 基于规则的查询优化(逻辑优化，Rule Based Optimization, 简称RBO)
- 基于代价的查询优化(物理优化，Cost Based Optimization, 简称CBO)

## 逻辑优化

## 物理优化

### 物理优化的4法宝

#### B+树

##### Hash表

#### 排序

#### 物化

### 物理路径生成

#### 物理路径分类

##### 顺序扫描

##### 索引扫描

##### 位图扫描

##### TID扫描

#### 物理连接路径

##### Nestlooped Join

##### HashJoin

##### SortMergeJoin



### 路径搜索方法

##### 自底向上

代表性的是System-R系统所使用的动态规划方法(最优解)。

##### 自顶向下

例如贪心算法(局部最右，复杂度低)。

##### 随机搜索

例如遗传算法。



查询优化的入口在`src\backend\optimizer\plan\planner.c`的`planner`它支持通过`planner_hook`来自定义优化方法，如果没有采用PG自定义的标准的查询优化函数`standard_planner`.

```c
PlannedStmt *
planner(Query *parse, const char *query_string, int cursorOptions,
		ParamListInfo boundParams)
```


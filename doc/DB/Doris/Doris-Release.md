# 2.1特性

## 高性能查询

## 存储增强

## 多模数据分析

## 智能负载均衡

## 数据湖分析



1. 面对大数据量、复杂场景的分析能力
   1. 优化器中增加了几十个查询优化规则，用SIMD重写部分函数实现
   2. 优化Shuffle的数据压缩方式
   3. 统计信息自动收集，Sample方式收集，无统计信息优化
2. 多表物化视图：加速多表关联查询、简化数据建模
3. Pipeline自适应
   1. Local Shuffle
      1. 减少部分情况下的数据倾斜
      2. 执行并发度不再受存储层tablet数据的制约
      3. 执行时进行动态并发
   2. 并行扫描（根据Tablet的大小自动拆分ScanTask）
4. 自增列（除agg模型）
5. auto Range Partition
6. Auto List Partition(例如字段的Hash值)
7. 服务端攒批，高并发写入更快更稳定
   1. 
8. Schema less的Variant类型（Schema-on-read结构）
9. 高性能数据湖访问
10. K8s Operator
11. 面相数据科学的高速数据读取
    1. MySQL文本协议
    2. 引入Arrow Flight实现高速数据读取
12. 资源隔离--硬限
    1. 多租户资源隔离(Resource Group)：机器分组
    2. WorkLoad Group:资源软限，硬限
13. 负载管理能力：自动分组、熔断
    1. workload schedule policy
14. 可观测行：TopSQL
    1. 进实时获取每个SQL在BE上的资源消耗（CPU，扫描数据量，Peak MEM，当前MEM）延迟大约3秒
15. 可视化Profile


# 新特性
## Doris 2.0.9新特性
新功能
- 物化视图的 Key 和 Value 列都允许出现谓词
- 物化视图支持bitmap_union(bitmap_from_array())
- 增加一个FE配置强制集群中所有表的Replicate Allocation
- 新优化器支持日期字面量指定时区
- MATCH_PHRASE全文检索支持 slop 参数指定搜索词之间的距离

改进与优化
- first_value/last_value函数增加第二个参数指定忽略 NULL 值
LEAD/LAG函数的 Offset 参数可以为 0
- 调整物化视图匹配的顺序优先利用索引和预聚合加速查询
- 优化TopN查询ORDER BY k LIMIT n 的性能
- 优化Meta Cache 的性能
- 为`delete_bitmap` `get_agg`函数增加Profile便于性能分析
- 增加FE参数设置Autobucket的最大Bucket数




> 当不同用户的多条语句同时在数据库中运行时，数据库需要巧妙地在这些用户这些语句中分配CPU、内存等资源，以便<b><font color=#FF5733>保证语句平稳运行</font></b>。资源管理需要在<b><font color=#FF5733>公平性、确定性和最大资源利用率之间保持微妙的平衡</font></b>，并且给予用户一些配置选择，以便适应不同的使用场景。<b><font color=#FF5733>公平性、确定性是指一个用户或者一个语句可以保证得到一部分资源</font></b>，这部分资源在任何时候都不会被剥夺，这样用户的这个语句就相当于被保留了一个最小可用资源集合，不论这个语句执行得早与晚、快与慢，都至少可以获得这部分资源。<b><font color=#FF5733>最大资源利用率</font></b>是指在系统相对空闲的时候，一个语句可以<b><font color=#FF5733>充分利用这些空闲的资源快速执行</font></b>完毕。
>
> 来自《Greenplum:从大数据战略到实现》7.3 资源管理

CPU和内存资源略有不同，CPU资源可以按照时间片调度，相对来说，调整一个进程的CPU使用占比可以快速完成。而内存资源一旦被分配并且使用，释放要么需要很长时间完成，要么实现起来太复杂，所以动态调整内存资源比较困难。

Greenplum 5实现了基于Linux cgroup的CPU资源管理，以及完全由Greenplum跟踪、记录和分配的内存资源管理，这种资源管理称为资源组(Resource Group)。资源组充分权衡了不同的实现方式，根据CPU和内存资源的不同特点实现了公平而高效的管理。
> cgroup是Linux内核的一个良好的资源分配特性，允许使用者<b><font color=#FF5733>创建不同的cgroup组，在配置文件限定CPU的使用量</font></b>。当用户将不同的进程对接到相应的cgroup组后，这些进程所能使用的CPU资源就会被限制在配置的数据之下或者附近，从而保证CPU资源在不同组之间的隔离。cgroup在系统其他组的CPU使用率低或者整个系统有空余的CPU资源时，还允许cgroup组的进程使用超出限额的CPU资源。


Greenplum 5.9中添加的一个新特性cpuset能够更好地保证短查询的资源。
```sql
-- 所有运行在rg3中的语句都会被调度到CPU核1上运行，且rg3中的语句对CPU核1是独占的
-- 其他组的语句只能被调度到其他核上
-- 当有一个短查询需要在rg3中运行时，会立刻在核1上运行，而不用担心要先把大查询的进程调度出来。
create resource group rg3 with (concurrency=3, cpuset='1', memory_limit=30);
```

# 管理指令

创建Resource Group需要`SUPERUSER`权限，最多创建100个。GP默认创建`admin_group`和`default_group`，这两个名称以及`none`被保留。

```sql
CREATE RESOURCE GROUP name WITH (group_attribute=value [, ... ])
```

其中，`group_attribute`的可能取值是

```sql
CPU_RATE_LIMIT=integer | CPUSET=tuple
[ MEMORY_LIMIT=integer ]
[ CONCURRENCY=integer ]
[ MEMORY_SHARED_QUOTA=integer ]
[ MEMORY_SPILL_RATIO=integer ]
[ MEMORY_AUDITOR= {vmtracker | cgroup} ]
```

| 属性                | 描述                                                         |      |
| ------------------- | ------------------------------------------------------------ | ---- |
| CONCURRENCY         | 资源组允许的最大并发数，包括活动的和空闲的事务，[0, MaxConnections(90)]。 |      |
| MEMORY_LIMIT        | 资源组可以分配MEMORY_LIMIT比例的Segment节点内存，取值范围为[0, 100]，<br/> 当MEMORY_LIMIT为0时，GreenPlum将不给资源组分配内存资源，使用资源组全局共享内存来满足所有内存请求。<br/>GP集群中所有资源组指定的MEMORY_LIMIT参数和不得超过100。 |      |
| MEMORY_AUDITOR      | 内存审计方式。默认，资源组使用vmtracker，外部组件使用cgroup  |      |
| MEMORY_SHARED_QUOTA | 资源组用于运行时事务之间共享的内存资源百分比<br/>将一个资源组内的内存分为两部分：组内各语句共享部分和每个语句独占的固定部分 |      |
| MEMORY_SPILL_RATIO  | 内存密集型事务的内存使用阈值。当事务达到此阈值时，它将溢出到磁盘。<br/>GP的Spill支持两种模式:<br/>1. Percentage模式，MEMORY_SPILL_RATIO的值不为0<br/>2. Fallback模式，由GUC参数statement_mem决定 |      |
| CPU_RATE_LIMIT      | 资源组的可用的CPU使用率，取值范围[0, 100]。                  |      |
| CPUSET              | 资源组保留的CPU核                                            |      |

SET，RESET和SHOW命令不受资源限制。



# 参考资料

1. 《Greenplum:从大数据战略到实现》

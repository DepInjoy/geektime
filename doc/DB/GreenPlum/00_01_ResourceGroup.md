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
| MEMORY_LIMIT        | 当前资源组预留(reserve)的内存资源百分比，取值范围为[0, 100]<br/> 当MEMORY_LIMIT为0时，GreenPlum将不给资源组分配内存资源，使用资源组全局共享内存来满足内存请求。<br/>GP集群中所有资源组指定的MEMORY_LIMIT参数和不得超过100。 |      |
| MEMORY_AUDITOR      | 内存审计方式。默认，资源组使用vmtracker，外部组件使用cgroup  |      |
| MEMORY_SHARED_QUOTA | 资源组用于运行时事务之间共享的内存资源百分比<br/>将一个资源组内的内存分为两部分：组内各语句共享部分和每个语句独占的固定部分 |      |
| MEMORY_SPILL_RATIO  | 内存密集型事务的内存使用阈值。当事务达到此阈值时，它将溢出到磁盘。<br/>GP的Spill支持两种模式:<br/>1. Percentage模式，MEMORY_SPILL_RATIO的值不为0<br/>2. Fallback模式，由GUC参数statement_mem决定 |      |
| CPU_RATE_LIMIT      | 资源组的可用的CPU使用率，取值范围[0, 100]。                  |      |
| CPUSET              | 资源组保留的CPU核                                            |      |

SET，RESET和SHOW命令不受资源限制。

# 资源组管理思想
资源组的内存管理是基于slot的方式进行管理的，在内存分配策略上采用了禁用OverCommit的方式，在内核上会配置
- `vm.overcommit_memory = 2`，系统默认为0，修改`/etc/sysctl.conf`进行配置
- `vm.overcommit_ratio`默认值为0.5

资源组实现了细粒度地对内存和CPU的管理。

## 内存管理
GP基于全局共享内存和slot机制对限制内存
> 启用资源组后，将在Greenplum数据库节点，段和资源组级别管理内存使用情况。
>
> 用户可以使用角色资源组在事务级别管理内存。gp_resource_group_memory_limit标识要分配给每个Greenplum数据库Segment主机上的资源组的系统内存资源的最大百分比，默认0.7(70%)。
>
> Greenplum数据库节点上可用的内存资源在节点上的每个Segment之间进一步平均分配。当基于资源组的资源管理处于活跃状态时，分配给段主机上每个段的内存量是Greenplum数据库可用的内存乘以gp_resource_group_memory_limit服务器配置参数，并除以主机上活跃Primary Segment的数量：
>
> rg_perseg_mem = ( (RAM * (vm.overcommit_ratio / 100) + SWAP) *  gp_resource_group_memory_limit) / num_active_primary_segments
>
>用户可以在创建资源组时指定MEMORY_LIMIT值来标识预留(reserve)用于资源管理的Segment内存的百分比。用户可以为资源组的MEMORY_LIMIT的取值范围是[0, 100]。
>
> 在Greenplum数据库群集中定义的所有资源组指定的MEMORY_LIMIT总和不得超过100。
>
> 参考资料：[GreenPlum Database Document : 用资源组进行工作负载管理](https://docs-cn.greenplum.org/v6/admin_guide/workload_mgmt_resgroups.html#topic8339777)

## CPU资源管理

GP提供了CPUSET和CPU_RATE_LIMIT两种资源组限制来标识CPU资源分配模式，配置资源组时只能选择其中一个限制，用户可以再运行时更改CPU资源的分配方式:
### 按核心数分配CPU资源
> 用户可以使用CPUSET属性标识要为资源组保留的CPU核心。用户指定的CPU核心必须在系统中可用，并且不能与为其他资源组保留的任何CPU核心重叠。将CPU核心分配给CPUSET组时，需要考虑以下事项：
>- 使用CPUSET创建的资源组仅使用指定的核心。如果组中没有正在运行的查询，则保留的核心处于空闲状态，并且其他资源组中的查询无法使用这些核心。 需要考虑最小化CPUSET组的数量以避免浪费系统CPU资源。
>
> - 考虑将CPU核心0不分配：在以下情况下，CPU核心0用作回退机制：
>     - admin_group和default_group至少需要一个CPU核心。 保留所有CPU内核后，Greenplum Database会将CPU内核0分配给这些默认组。 在这种情况下，用户为其分配CPU核心0的资源组与admin_group和default_group共享核心。
>     - 如果通过一个节点替换重新启动Greenplum数据库集群，并且节点没有足够的内核来为所有CPUSET资源组提供服务，则会自动为这些组分配CPU核心0以避免系统启动失败。
>- 尽可能将较小的核心编号分配给资源组。 如果替换Greenplum数据库节点并且新节点的CPU核心数比原始节点少，或者备份数据库并希望在具有较少CPU核心的节点的群集上做恢复，操作可能会失败。 例如，如果用户的Greenplum数据库群集有16个核心，则分配核心1-7是最佳选择。 如果创建资源组并分配CPU核心9，则数据库恢复到8核心的节点将失败。
>
> 为资源组配置CPUSET时，Greenplum数据库会禁用组的CPU_RATE_LIMIT并将值设置为-1。

### 按百分比分配CPU资源
> Greenplum节点CPU百分比在Greenplum节点上的每个Segment间平均分配。 使用CPU_RATE_LIMIT配置每个资源组保留用于资源管理的段CPU的指定百分比，其取值范围是[1, 100]。Greenplum数据库群集中定义的所有资源组指定的CPU_RATE_LIMIT的总和不得超过100。
>
> 按百分比分配核心(资源组指定CPU_RATE_LIMIT)，Segment主机上配置的所有资源组的最大CPU资源使用量是以下值中的最小值：
> - 非保留CPU核心数除以所有CPU核心数乘以100，和
> - gp_resource_group_cpu_limit值。
>
> 配置有CPU_RATE_LIMIT的资源组的CPU资源分配是弹性的，因为Greenplum数据库可以将空闲资源组的CPU资源分配给更繁忙的资源组。在这种情况下，当该资源组接下来变为活动时，CPU资源被重新分配给先前空闲的资源组。如果多个资源组繁忙，则根据其CPU_RATE_LIMIT的比率为它们分配任何空闲资源组的CPU资源。例如，使用CPU_RATE_LIMIT为40创建的资源组将分配两倍于使用CPU_RATE_LIMIT为20创建的资源组的额外CPU资源。
>
> 为资源组配置CPU_RATE_LIMIT时，禁用资源组的CPUSET并将值设置为-1。

# 源码解读

基于最新的`6.26.x`分支进行源码解读。源码实现层面主要涉及两个文件：

| 文件                                              | 职责                 |
| ------------------------------------------------- | -------------------- |
| `src/backend/commands/resgroupcmds.c`             | 资源组管理指令       |
| `src/backend/utils/resgroup/resgroup.c`           | 资源组管理实现核心   |
| `src/backend/utils/resgroup/resgroup-ops-linux.c` | Linux cgroup相关操作 |

## 初始化

### 内存初始化
GP将所有资源放在一个全局的mem pool中，通过`ResGroupControl *pResGroupControl`结构来维护相关的数据信息
- `chunkSizeInBits`一个chunk多少bit
- `totalChunks`一共有多少chunk
- `freeChunks`空闲chunk数量，代表全局共享内存的大小。
- `safeChunksThreshold100`表示Safe memory，当全局共享资源小于该阈值的时候，资源组的内存使用进入red zone

```c
void InitResGroups(void) {
					......                
	// 1. 计算Segment上的chunk总数
	//    计算可管理的chunk总数(totalChunks),一个chunk容量(chunkSizeInBits)
	decideTotalChunks(&pResGroupControl->totalChunks, &pResGroupControl->chunkSizeInBits);
	pg_atomic_write_u32(&pResGroupControl->freeChunks, pResGroupControl->totalChunks);
	pg_atomic_write_u32(&pResGroupControl->safeChunksThreshold100,
						pResGroupControl->totalChunks * (100 - runaway_detector_activation_percent));
	// 2. cgroup初始化
    ResGroupOps_Init();
    				.......
}
```
通过`decideTotalChunks`接口，GP将这些内存划分为不超过16K的chunk，之后基于Chunk进行管理。默认一个chunk是1M，默认的一个Chunk为1M大小，如果可用虚拟内存大于16GB(16K MB)，则通过增大一个Chunk的大小(依次放大为2M、4M....)，来确保Chunk的数量不超过16K。
```C
// Calculate the total memory chunks of the segment
static void decideTotalChunks(int32 *totalChunks, int32 *chunkSizeInBits) {
	int32 nsegments;
	int32 tmptotalChunks;
	int32 tmpchunkSizeInBits;

	// 获取primary segments数
	nsegments = Gp_role == GP_ROLE_EXECUTE ? host_segments : pResGroupControl->segmentsOnMaster;
	/**
	 * 结合Linux系统配置以及cgroup信息获取可管理内存, 单位MB
	 * 	
	 *  Linux系统 outTotalMemory = SWAP + RAM * vm.overcommitRatio / 100
	 * 
	 *  通过getCgMemoryInfo中获取cgroup的RAM和swap
	 * 		cgram(MEM) : memory.limit_in_bytes
	 * 		cgmemsw(SWAPs): memory.memsw.limit_in_bytes
	 * 
	 * total = Min(outTotalMemory, (cgmemsw<memsw ? cgmemsw - ram : swap) + Min(ram, cgram))
	 * */
	tmptotalChunks = ResGroupOps_GetTotalMemory() * gp_resource_group_memory_limit / nsegments;

	// 默认一个chunk含1M
	tmpchunkSizeInBits = BITS_IN_MB;
	// 如果vmem > 16GB，每个chunk含的bit容量(chunkSizeInBits)增大
	// 确保每个chunk单元的vmem不超过16K
	while(tmptotalChunks > (16 * 1024)) {
		tmpchunkSizeInBits++;
		tmptotalChunks >>= 1;
	}

	*totalChunks = tmptotalChunks;
	*chunkSizeInBits = tmpchunkSizeInBits;
}
```

### CPU管理初始化
Linux通过cgroup对CPU的控制，通过比例隔离资源。按每个分组里面`cpu.shares`的比率来分配cpu，比如A B C三个分组,cpu.shares分别设置为1024/1024/2048,那么他们可以使用的cpu比率为1:1:2。初始化cgroup，将postmaster和子进程加入`gpdb cgroup`。

```C++
/* Initialize the OS group */
void ResGroupOps_Init(void) {
    // 1. 初始化CPU设置
    //    cfs_quota_us := parent.cfs_period_us * ncores * gp_resource_group_cpu_limit
    //    shares := parent.shares * gp_resource_group_cpu_priority
    initCpu();

    // 2. 初始化cpuset设置
    //    从cgroup cpuset root路径下获取到cpuset信息写入gpdb路径下
    //    相关配置有cpuset.mems和cpuset.cpus，分别对应于
    //    cpuset/gpdb/cpuset.mems和cpuset/gpdb/cpuset.cpus
    initCpuSet();

	/*
	 * Put postmaster and all the children processes into the gpdb cgroup,
	 * otherwise auxiliary processes might get too low priority when
	 * gp_resource_group_cpu_priority is set to a large value
	 */
	ResGroupOps_AssignGroup(RESGROUP_ROOT_ID, NULL, PostmasterPid);
}
```


# 参考资料

1. 《Greenplum:从大数据战略到实现》
2. [GreenPlum Database Document : 用资源组进行工作负载管理](https://docs-cn.greenplum.org/v6/admin_guide/workload_mgmt_resgroups.html#topic8339777)

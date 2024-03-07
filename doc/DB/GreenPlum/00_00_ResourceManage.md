Greenplum目前支持对内存和CPU两种资源进行管理，同时支持两种形式的资源管理策略：Resource Queue(RQ)和Resource Group(RG)。二者都是基于角色的策略，即把用户分成若干个组，每个组对应不同的策略。二者都可以对CPU、内存和并发数进行控制，默认采用资源队列。

| 度量标准 | 资源队列                                           | 资源组                                                       |
| :------- | :------------------------------------------------- | :----------------------------------------------------------- |
| 并发度   | 查询级管理                                         | 事务级管理                                                   |
| CPU      | 指定查询优先级管理CPU                              | 通过Linux cgroup以百分率为单位管理CPU          |
| 内存     | 查询级内存管理，用户可以显式声明方式突破内存上限 | 事务级内存管理，用户无法突破内存上限                         |
| 内存隔离 | （无）                                             | 资源组之间以及同一个资源组内部的事务间内存是隔离的           |
| 用户     | 仅限制非管理员用户                                 | 限制适用于超级用户和非管理员用户                             |
| 排队     | 仅限没有slot的队列                                 | 没有slot的或者没有可用内存的队列                             |
| 队列失败 | 如果没有足够内存，队列可能立即失败。               | 当没有共享资源组内存和事务需要更多内存时，<br/>查询达到事务固定内存限制后，可能会失败。 |
| 限制旁路 | 超级用户角色和特定的操作及函数不受限制             | SET、RESET和SHOW命令不受限制                                 |
| 外部组件 | 无法管理外部组件使用的内存                         | 通过cgroup管理外部组件PL/Container的CPU和内存资源              |

GP通过GUC配置参数决定资源管理方式

| 配置参数                              | 描述                                                         | 默认值  |
| ------------------------------------- | ------------------------------------------------------------ | ------- |
| `resource_scheduler`                  | 是否使能资源调度方案，默认为true<br/><br/>`resource_manager.c`存在一个全局参数`bool ResourceScheduler `进行表示。 | `true`  |
| `gp_resource_manager`                 | 采用的资源管理方案，支持资源队列和资源组，默认使用资源队列。可选项`queue`，`group`<br/><br/>`resource_manager.c`存在一个全局参数`Gp_resource_manager_policy`表示资源调度策略<br/>1. 资源队列，RESOURCE_MANAGER_POLICY_QUEUE<br/>2. 资源组，RESOURCE_MANAGER_POLICY_GROUP | `queue` |
| `runaway_detector_activation_percent` | 百分比(整数)，仅在资源管理设置为资源队列时，才生效<br/>设置触发查询终止的Greenplum数据库vmem内存的百分比。<br/>如果用于Greenplum数据库segment的vmem内存百分比超过指定值， Greenplum数据库将根据内存使用情况，从占用最大内存量的查询开始终止查询。 | 90      |



GP提供了一下几个参数来配置GP允许使用的CPU和内存的比例

| 配置参数                       | 默认值 | 描述                                                         |
| ------------------------------ | :----: | ------------------------------------------------------------ |
| gp_resource_group_memory_limit |  0.7   | 分配给 Greenplum 数据库的系统内存百分比，默认70%。           |
| gp_resource_group_cpu_limit    |  0.9   | 分配给每个Greenplum数据库Segment上的资源组的系统CPU资源的最大百分比。<br/>无论资源组CPU分配模式如何，此限制都将控制Segment主机上所有资源组的最大CPU使用率。<br/>剩余的未预留CPU资源用于OS内核和Greenplum数据库辅助守护进程 |
| gp_resource_group_cpu_priority |   10   | postgres进程的cpu优先级                                      |

```C++
// GUC配置参数
// 是否使能资源调度方案(resource_scheduler)
bool ResourceScheduler = false;
// 采用的资源管理方案(gp_resource_manager)
ResourceManagerPolicy Gp_resource_manager_policy;

// 全局变量，变量值更改参见resource_manager.c的InitResManager
bool ResGroupActivated = false;
```

# 初始化资源管理

```C
/**
 * 初始化资源管理的shared memory
 *     通过GUC配置参数决定资源调度方式
 * 	   1. ResourceScheduler:是否使能资源调度方案，默认为false
 * 	   2. Gp_resource_manager_policy资源管理策略,目前支持:
 *          2.1 RESOURCE_MANAGER_POLICY_QUEUE:资源队列
 *          2.2 RESOURCE_MANAGER_POLICY_GROUP: 资源组
 */
void ResManagerShmemInit(void) {
	if (IsResQueueEnabled() && Gp_role == GP_ROLE_DISPATCH) {
		InitResScheduler();
		InitResPortalIncrementHash();
	} else if (IsResGroupEnabled() && !IsUnderPostmaster) {
		ResGroupControlInit();
	}
}
```

> Red Hat Enterprise Linux 6 provides a new kernel feature: control groups, which are called by their shorter name cgroups in this guide. Cgroups allow you to allocate resources — such as CPU time, system memory, network bandwidth, or combinations of these resources — among user-defined groups of tasks (processes) running on a system. You can monitor the cgroups you configure, deny cgroups access to certain resources, and even reconfigure your cgroups dynamically on a running system. The cgconfig (control group config) service can be configured to start up at boot time and reestablish your predefined cgroups, thus making them persistent across reboots.
> 
> By using cgroups, system administrators gain fine-grained control over allocating, prioritizing, denying, managing, and monitoring system resources. Hardware resources can be appropriately divided up among tasks and users, increasing overall efficiency.

Red Hat Enterprise Linux 6提​​​供​​​新​​​的​​​内​​​核​​​功​​​能​​​：控​​​制​​族​​​群​​​(control group)，​​​简​​​称​​​cgroup。​​​Cgroup可以为运行的系统中一组用户自定义任务(进程)分配资源，例如CPU时间，系统内存，网络带宽，或这些资源组合。用户可以监控其配置的cgroup，拒绝cgroup访问特定资源，设置在运行时动态配置cgroup。cgconfig(control group config,控制族配置)服务，在引导时启动，并重新建立预定义的cgroup，使得在重启过程中保留用户预定义的cgroup。

使用cgroup，系统管理员可以对系统资源的分配、优先级、拒绝、管理和监控进行细粒度的控制。硬件资源可以在任务和用户之间适当分配，提高整体效率。

---
Red Hat Enterprise Linux可用的子系统(subsystem)：
- blkio，该子系统为块设备设定输入/输出限制，例如，物理设备(磁盘，固态硬盘，USB等)。
- cpu，该子系统使用调度程序提供cgroup任务访问CPU。
- cpuacct，该子系统自动生成cgroup中任务所使用的CPU报告。
- cpuset，该子系统为cgroup中的任务分配独立的CPU核心(多核心系统)和节点内存。
- devices，该​子​​​系​​​统​​​可​​​允​​​许​​​或​​​者​​​拒​​​绝​​​ cgroup 中​​​的​​​任​​​务​​访​​​问​​​设​​​备​
- freezer，挂起或恢复cgroup中的任务
- memory，​设​​​定​​​ cgroup 中​​​任​​​务​​​使​​​用​​​的​​​内​​​存​​​限​​​制​​​，并​​​自​​​动​​​生​​​成​​这些​​任​​​务​​​使​​​用​​​的​​​内​​​存​​​资​​​源​​​报​​​告​​​。
- net_cls，使​​​用​​​等​​​级​​​识​​​别​​​符​​​(classid)标​​​记​​​网​​​络​​​数​​​据​​​包​​​，​​​允​​​许​​​Linux流​​​量​​​控​​​制​​​程​​​序​​​(tc)识​​​别​​​从​​​特定cgroup中​​​生​​​成​​​的​​​数​​​据​​​包​​​。
- net_prio，提供了一种动态设定每个网络接口的网络流量优先级的方法。
- ns，命名空间(namespace)子系统。
- perf_event，该子系统可以识别cgroup任务成员，可用于性能分析。

# 子系统和控制参数

## CPU子系统

cpu子​​​系​​​统​​​调​​​度​​​对​​​cgroup的​​CPU访​​​问​​​。下面两种调度策略可以实现对CPU资源的访问:
- CFS(ompletely Fair Scheduler，完全公平调度器)，比例份额调度策略(proportional share scheduler)，根据任务的优先级/权重或分配给cgroup的份额(share)，在任务组(cgroup)之间按比例分配CPU时间(CPU带宽)。
- RT(Real-Time scheduler)，一种指定实时任务可以使用的CPU时间任务调度策略。

### CFS控制参数

下面的参数可用于配置强制上限(ceiling enforcement)和相对的共享份额(relative sharing)：

---
强制上限(ceiling enforcement)控制参数
- `cpu.cfs_period_us`，指定一个时间段用于重新分配cgroup对CPU资源的访问的频率，单位微秒。如果cgroup中的任务能够每1秒访问单个CPU 0.2秒，将`cpu.cfs_quota_us`设置为200000，`cpu.cfs_period_us`设置为1000000。cpu.cfs_quota_us参数的上限为 1秒，下限为1000微秒。
<br/>
- `cpu.cfs_quota_us`，指定cgroup中的所有任务在某个周期(由`cpu.cfs_period_us`指定)内可以运行的总时间，单位为微秒。一旦cgroup中的任务用完了指定配额的所有时间，它们就会在指定周期剩余时间内受到限制，并且不允许在下一个周期之前运行。如果cgroup中的任务应该能够每1秒访问单个CPU 0.2秒，将`cpu.cfs_quota_us`设置为200000，`cpu.cfs_period_us`设置为 1000000。<br/>
将`cpu.cfs_quota_us`中的值设置为-1表示cgroup不受任何CPU时间限制。这是除根cgroup以外，所有cgroup的默认值。
<br/>
- `cpu.stat`，使用下面参数参数报告CPU时间统计信息。
    - `nr_periods`，以过去的周期间隔(由`cpu.cfs_period_us`指定)数
    - `nr_throttled`，cgroup中的任务受到限制的次数(即不允许运行，因已用尽其配额指定的所有可用时间)。
    - `throttled_time`，cgroup中的任务受到限制的总持续时间(单位：纳秒)。
---
相对共享份额(relative sharing)控制参数

- `cpu.shares`，包含一个整数值，该值指定cgroup中的任务可用的CPU时间的相对份额。例如，两个将`cpu.shares`设置为100的cgroup中的任务将获得相等的CPU时间，但将`cpu.shares`为200的cgroup中的任务获得的CPU时间是将`cpu.shares`为100的两倍。`cpu.shares`的值必须大于等于2.

### RT控制参数

## cpuset子系统

# 参考资料
1. [Red Hat:控​​​制​​​组​​​群​​​(Cgroup)简​​​介](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/resource_management_guide/ch01)
2. [Red Hat - Resource Manage : cpu](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/resource_management_guide/sec-cpu)
3. [Red Hat - Resource Manage : cpuset](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/resource_management_guide/sec-cpuset)
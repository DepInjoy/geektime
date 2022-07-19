

内存管理从以下几个方向来了解：
1. 内存管理的目标
2. 虚拟内存的概念。
3. 内存分层
4. 操作系统在内存中的位置。如何将内存空间在操作系统和用户程序之间进行分配？

## 内存管理的目标
内存管理要达到两个目标：
1. **地址保护：一个程序不能访问另一个程序地址空间。** 由于多道程序同时存放在内存中，操作系统要保证它们之间互不干扰。所谓的互不干扰就是一个进程不能随便访问另一个进程的地址空间。
2. **地址独立：程序发出的地址应与物理主存地址无关。** 程序指令在执行前加载到内存，然后从内存中一条条指令读出，然后执行（从硬件层来看，指令的“读取-执行”循环是计算机的基本操作）。每条指令在执行时需要读取操作数和写入运算结果，要读取操作数，就需要给出操作数所在的内存地址，这个地址不能是物理主存地址。这是因为该程序在何种硬件配置的机器上运行并不能事先确定。因此，指令里面的地址是程序空间（虚拟空间）的虚拟地址（程序地址）。即程序发出的地址与具体机器的物理主存地址是独立的。

## 虚拟内存
程序的运行必须加载到物理主存中，而物理主存容量有限。因此，如果要把一个程序全部加载到物理主存，它的最大容量受制于主存容量(还要减去操作系统所占的空间和一些临时缓存空间);即使我们编写的每个程序的尺寸都小于物理主存容量，主存能够存放的程序数量将是很有限的，而这将极大地限制多道编程的发展。如何解决物理主存容量偏小的缺陷呢？最简单的办法就是购买更大的物理主存。而这将造成计算机成本的大幅飙升。如何在不增加成本的情况下扩大内存容量？这就是虚拟内存。

> 虚拟内存的中心思想是将物理主存扩大到便宜、大容量的磁盘上，即将磁盘空间看做主存空间的一部分。用户程序存放在磁盘上就相当于存放在主存内。用户程序既可以完全存放在主存，也可以完全存放在磁盘上，当然也可以部分存放在主存、部分存放在磁盘。而在程序执行时，程序发出的地址到底是在主存还是在磁盘则由操作系统的内存管理模块负责判断，并到相应的地方进行读写操作。**虚拟内存要提供的就是一个空间像磁盘那样大、速度像缓存那样高的主存储系统。**

## 操作系统在内存中的位置
<center>
    <img src="img/op-in-mem-no-rom.png">
</center>
对于图中a, 操作系统处于地址空间下面还有一个实际好处：就是在复位、中断、陷入等操作时，控制移交给操作系统更方便，因为操纵系统的起始地址为0，无须另行记录操作系统所处的位置，程序计数器清零就可以了。清零操作对于硬件来说非常简单，无须从总线或寄存器读取任何数据

对于图中b，操作系统为用户提供服务的，在逻辑上处于用户程序之下。将其置于地址空间的下面，符合人们的惯性思维。

<center>
    <img src="img/op-in-mem-with-rom.png">
</center>
对于图中a，操作系统放在ROM里面的好处是不容易被破坏，缺点就是ROM要做得大，能够容纳整个操作系统。

对于图中b，由于ROM比较贵，通常情况下是备有少量的ROM，只将操作系统的一部分放在ROM里，其余部分放在RAM里。同时将输入输出和内存访问统一起来。即将输入输出设备里面的寄存器或其他存储媒介编入内存地址（在用户程序地址之上），使得访问输入输出设备如同访问内存一样。这种输入输出称为内存映射的输入输出。如果要访问的地址高于RAM的最高地址，则属于I/O操作，否则属于正常内存操作。

根据操作系统是否占用ROM或我们是否采用内存映射的输入输出来分，存在两种模式：
1. 操作系统占用RAM的底层，用户程序占用RAM的上层。
2. 操作系统占用RAM的底层和位于用户程序地址空间上面的ROM，用户程序位于中间。该模式又分为3种情况：
    1) 没有使用内存映射的输入输出，ROM里面全部是操作系统。CP/M操作系统的内存布局模式属于这种情况，其BIOS和CP/M内核均处于ROM里面，而shell和用户程序处于RAM里。
    2) 使用了内存映射的输入输出，ROM的一部分是操作系统，另一部分属于I/O设备。大多数操作系统采用这种模式。
    3) 使用了内存映射的输入输出，ROM全部属于I/O设备。

> 每个进程都有自己独立的地址空间，这些地址空间都是虚拟地址，就像是镜中花，水中月。当进程需要内存时（例如通过brk分配内存），进程从内核获得的仅仅是一段虚拟地址的使用权，而不是实际的物理地址，进程并没有获得物理内存。实际的物理内存只有当进程真的去访问新获取的虚拟地址时，产生“缺页”异常，从而进入分配实际物理地址的过程，也就是分配实际的page frame并建立page table。之后系统返回产生异常的地址，重新执行内存访问，一切好象没有发生过。因此，看起来虚拟内存和物理内存的分配被分割开了，这是否意味着进程可以任意的申请虚拟地址空间呢？也不行，毕竟virtual memory需要physical memory做为支撑，如果分配了太多的virtual memory，和物理内存不成比例，对性能会有影响。对于这个状况，我们称之为overcommit[2]。

## 内存分配策略
Linux大部分申请内存的请求都回复"yes"，以便能跑更多更大的程序。因为申请内存后，并不会马上使用内存。这种技术叫做`OverCommit`。 当内存不足时，会发生`OOM。`它会选择杀死一些进程(用户态进程，不是内核线程)，以便释放内存。Linux支持三种[`OverCommit`策略](https://www.kernel.org/doc/Documentation/vm/overcommit-accounting)。

内存分配策略，相关参数`vm.overcommit_memory`

> 参数说明 ：位于`/proc/sys/vm/overcommit_memory`可选值：0、1、2。 
>
> - 0 :  启发式策略(`Heuristic overcommit handling`)，是系统的默认值。明显的过度使用地址空间会被拒绝，它在确保不合理的内存分配失败的同时允许过度使用来减少swap。在该模式下，root用户可以分配稍微多一些的内存。
>
> - 1 : 任何`OverCommit`都会被接受。在kernel的官方介绍有如下的描述：
>
>     > Always overcommit. Appropriate for some scientific applications. Classic example is code using sparse arrays and just relying on the virtual memory consisting almost entirely of zero pages.
>
> - 2 : 禁止OverCommit。总地址空间超用不允许超过swap + `vm.overcommit_ratio`(默认50)/100 * RAM，该模式下，大多数情况下进程在访问页不会被kill，而在申请内存时报错。该模式会降低内存的使用效率，浪费内存资源。其中`overcommit`的量取决于`vm.overcommit_kbytes`(绝对值)和`vm.overcommit_ratio`(百分数) 关于使用场景在kernel的官方介绍有如下的描述
>
>     > Useful for applications that want to guarantee their memory allocations will be available in the future without having to initialize every page.



可以通过`/proc/meminfo`的`CommitLimit`查询当前系统的`overcommit limit`以及`Committed_AS`查询已经分配内存大小。




如何查询和设置内存分配策略，它由`sysctl`指令支持对其操作

```shell
# 查询vm.overcommit_memory参数
sysctl vm.overcommit_memory
sysctl -a | grep vm.overcommit_memory
cat /proc/sys/vm/overcommit_memory

# 修改vm.overcommit_memory参数，例如将其修改为1
sysctl vm.overcommit_memory=1
echo 1 > /proc/sys/vm/overcommit_memory
```



## OOM

> 当oom-killer发生时，linux会选择杀死哪些进程 选择进程的函数是oom_badness函数(在mm/oom_kill.c中)，该函数会计算每个进程的点数(0~1000)。 点数越高，这个进程越有可能被杀死。 每个进程的点数跟oom_score_adj有关，而且oom_score_adj可以被设置(-1000最低，1000最高)[1]。

>/proc/[pid]/oom_score (since Linux 2.6.11)
>
>This file displays the current score that the kernel gives to this process for the purpose of selecting a process for the OOM-killer.  A higher score means that the process is more likely to be selected by the OOM-killer.  The basis for this score is the amount of memory used by the process, with increases (+) or decreases (-) for factors including.
>* whether the process is privileged (-).
>Before kernel 2.6.36 the following factors were also used in the calculation of oom_score:
>* whether the process creates a lot of children usingfork(2) (+);
>* whether the process has been running a long time, or has used a lot of CPU time (-);
>* whether the process has a low nice value (i.e., > 0) (+); and
>* whether the process is making direct hardware access (-).
>
>The oom_score also reflects the adjustment specified by the oom_score_adj or oom_adj setting for the process.[3]

## 内存使用信息

`/proc/meminfo`是了解Linux系统内存使用状况的主要接口。

[/PROC/MEMINFO之谜](http://linuxperf.com/?p=142)



# 参考资料

1. [sysctl 中 vm.overcommit_memory 的含义](https://blog.51cto.com/lookingdream/1933132)
2. [Linux vm运行参数之(一):overcommit相关的参数](http://www.wowotech.net/memory_management/overcommit.html)
3. [proc(5) — Linux manual page](https://man7.org/linux/man-pages/man5/proc.5.html)


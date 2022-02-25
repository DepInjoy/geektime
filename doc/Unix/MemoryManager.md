

# 内存管理

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



## 内存使用信息

`/proc/meminfo`是了解Linux系统内存使用状况的主要接口。

[/PROC/MEMINFO之谜](http://linuxperf.com/?p=142)



# 参考资料

1. [sysctl 中 vm.overcommit_memory 的含义](https://blog.51cto.com/lookingdream/1933132)
2. [Linux vm运行参数之(一):overcommit相关的参数](http://www.wowotech.net/memory_management/overcommit.html)
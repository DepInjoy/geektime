1. `SIGCONT`信号 : 让进程继续执行

> You can send a SIGCONT signal to a process to make it continue. This signal is special—it always makes the process continue if it is stopped, before the signal is delivered. The default behavior is to do nothing else. You cannot block this signal. You can set a handler, but SIGCONT always makes the process continue regardless.> 
> 
> Most programs have no reason to handle SIGCONT; they simply resume execution without realizing they were ever stopped. You can use a handler for SIGCONT to make a program do something special when it is stopped and continued—for example, to reprint a prompt when it is suspended while waiting for input.
> 
> 来自 [Job Control Signals](https://www.gnu.org/software/libc/manual/html_node/Job-Control-Signals.html)



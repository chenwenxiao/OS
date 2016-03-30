##请在ucore启动时显示空闲进程（idleproc）和初始进程（initproc）的进程标识。

在kern/init/init.c中添加输出idleproc和initproc的pid的代码即可

    cprintf("idleproc = %d, initproc = %d\n", idleproc->pid, initproc->pid);

可以参见代码[init.c](https://github.com/chenwenxiao/OS/blob/master/hw10/init.c) 

##(spoc)设计一个简化的进程管理子系统，可以管理并调度如下简化进程.给出了参考代码，请理解代码，并完成＂YOUR CODE"部分的内容．
可以参见代码[process-cpuio-homework.py](https://github.com/chenwenxiao/OS/blob/master/hw10/process-cpuio-homework.py) 

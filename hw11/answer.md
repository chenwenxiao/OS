### (1) ucore的线程控制块数据结构是什么？
    struct proc_struct {
        enum proc_state state;                      // Process state
        int pid;                                    // Process ID
        int runs;                                   // the running times of Proces
        uintptr_t kstack;                           // Process kernel stack
        volatile bool need_resched;                 // bool value: need to be rescheduled to release CPU?
        struct proc_struct *parent;                 // the parent process
        struct mm_struct *mm;                       // Process's memory management field
        struct context context;                     // Switch here to run process
        struct trapframe *tf;                       // Trap frame for current interrupt
        uintptr_t cr3;                              // CR3 register: the base addr of Page Directroy Table(PDT)
        uint32_t flags;                             // Process flag
        char name[PROC_NAME_LEN + 1];               // Process name
        list_entry_t list_link;                     // Process link list 
        list_entry_t hash_link;                     // Process hash list
    };

### (2) 如何知道ucore的两个线程同在一个进程？
同一个进程的页目录位置是相同的，只需要判断cr3或者mm_struct是否相同

### (3) context和trapframe分别在什么时候用到？
context用于切换进程，线程时要用到，而trapframe用作中断处理

### (4) 用户态或内核态下的中断处理有什么区别？在trapframe中有什么体现？
用户态要转移到内核态时，需要特权级的变换，trapframe中要压入ss和esp，用于切换内核栈。
而内核态到内核态不需要，栈中只要压入eflag,cs,eip,errorcode就可以了

### (5) do_fork中的内核线程执行的第一条指令是什么？它是如何过渡到内核线程对应的函数的？
    tf.tf_eip = (uint32_t) kernel_thread_entry;

在copy_thread中,将*(proc->tf)修改为*tf，意味着tf_eip，也就是eip被改为了指向kernel_thread_entry的指针。
那么再执行时，就会跳转到kernel_thread_entry的位置。

执行的第一条内核指令就是pushl %edx

    .text
    .globl kernel_thread_entry
    kernel_thread_entry:        # void kernel_thread(void)

        pushl %edx              # push arg
        call *%ebx              # call fn

        pushl %eax              # save the return value of fn(arg)
        call do_exit            # call do_exit to terminate current thread
 
### (6)内核线程的堆栈初始化在哪？

这里初始化了内核线程的对战

    static void
    copy_thread(struct proc_struct *proc, uintptr_t esp, struct trapframe *tf) {
        proc->tf = (struct trapframe *)(proc->kstack + KSTACKSIZE) - 1;
        *(proc->tf) = *tf;
        proc->tf->tf_regs.reg_eax = 0;
        proc->tf->tf_esp = esp;
        proc->tf->tf_eflags |= FL_IF;

        proc->context.eip = (uintptr_t)forkret;
        proc->context.esp = (uintptr_t)(proc->tf);
    }

### (7)fork()父子进程的返回值是不同的。这在源代码中的体现中哪？
父进程的返回值是proc->pid，说明是返回创建的子线程的编号，而子线程的返回值在栈初始化时规定为

	proc->tf->tf_regs.reg_eax = 0;

### (8)内核线程initproc的第一次执行流程是什么样的？能跟踪出来吗？

在proc_init中先完成了初始化，在kern_init最后调用cpu_idle切换到内核线程。cpu_idle会检查need_resched， 如果为1那么进行调度。

在schedule中进行切换线程，要切换到RNNING时执行proc_run，在proc_run中调用了switch_to来完成切换。

在完成进程后，会进入kernel_thread_entry，然后返回返回值，并do_exit。

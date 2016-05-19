### 1）小组思考题(1-键盘输入)：描述ucore操作系统中“键盘输入”从请求到完成的整个执行过程，并分析I/O过程的时间开销。

在这里引起硬件中断，在trap.c中的trap_dispatch处理了键盘中断，并且读入了字符c

    case IRQ_OFFSET + IRQ_KBD:
        c = cons_getc();
        //cprintf("kbd [%03d] %c\n", c, c);
        {
          extern void dev_stdin_write(char c);
          dev_stdin_write(c);
        }
        break;
        
然后进入dev_stdin_write处理字符c。

    void
    dev_stdin_write(char c) {
        bool intr_flag;
        if (c != '\0') {
            local_intr_save(intr_flag);
            {
                stdin_buffer[p_wpos % STDIN_BUFSIZE] = c;
                if (p_wpos - p_rpos < STDIN_BUFSIZE) {
                    p_wpos ++;
                }
                if (!wait_queue_empty(wait_queue)) {
                    wakeup_queue(wait_queue, WT_KBD, 1);
                }
            }
            local_intr_restore(intr_flag);
        }
    }
    
其中屏蔽了中断，之后将输出存储在stdin_buffer中，如果等待队列非空，那么唤醒在等待键盘的进程。

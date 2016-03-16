#Lab1

##练习1：理解通过make生成执行文件的过程。（要求在报告中写出对下述问题的回答）

###列出本实验各练习中对应的OS原理的知识点，并说明本实验中的实现部分如何对应和体现了原理中的基本概念和关键知识点。在此练习中，大家需要通过静态分析代码来了解：操作系统镜像文件ucore.img是如何一步一步生成的？(需要比较详细地解释Makefile中每一条相关命令和命令参数的含义，以及说明命令导致的结果)

```
# create ucore.img
UCOREIMG	:= $(call totarget,ucore.img)

$(UCOREIMG): $(kernel) $(bootblock)
	$(V)dd if=/dev/zero of=$@ count=10000
	$(V)dd if=$(bootblock) of=$@ conv=notrunc
	$(V)dd if=$(kernel) of=$@ seek=1 conv=notrunc

$(call create_target,ucore.img)
```

其中代码表示
总共生成10000个块
把bootblock的内容写到最开始的块中
把kernel的内容写到最开始的块的后一块中


这就是生成ucore.img的代码，可以看到$(kernel) $(bootblock)，也就是说之前要生成kernel,bootblock

```
# create kernel target
kernel = $(call totarget,kernel)

$(kernel): tools/kernel.ld

$(kernel): $(KOBJS)
	@echo + ld $@
	$(V)$(LD) $(LDFLAGS) -T tools/kernel.ld -o $@ $(KOBJS)
	@$(OBJDUMP) -S $@ > $(call asmfile,kernel)
	@$(OBJDUMP) -t $@ | $(SED) '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(call symfile,kernel)

$(call create_target,kernel)
```

kernel之前需要生成kernel.ld和KOBJS

kernel.ld为已存在工具，KOBJS通过以下来生成


```
$(call add_files_cc,$(call listf_cc,$(KSRCDIR)),kernel,$(KCFLAGS))

KOBJS	= $(call read_packet,kernel libs)
```

到这里kernel的生成结束了，之后是bootblock


```
# create bootblock
bootfiles = $(call listf_cc,boot)
$(foreach f,$(bootfiles),$(call cc_compile,$(f),$(CC),$(CFLAGS) -Os -nostdinc))

bootblock = $(call totarget,bootblock)

$(bootblock): $(call toobj,$(bootfiles)) | $(call totarget,sign)
	@echo + ld $@
	$(V)$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 $^ -o $(call toobj,bootblock)
	@$(OBJDUMP) -S $(call objfile,bootblock) > $(call asmfile,bootblock)
	@$(OBJCOPY) -S -O binary $(call objfile,bootblock) $(call outfile,bootblock)
	@$(call totarget,sign) $(call outfile,bootblock) $(bootblock)

$(call create_target,bootblock)
```

bootblock之前需要生成bootfiles，sign，其中bootfiles在之前几行完成
而sign则由这几个命令完成

```
# create 'sign' tools
$(call add_files_host,tools/sign.c,sign,sign)
$(call create_target_host,sign,sign)
```


###一个被系统认为是符合规范的硬盘主引导扇区的特征是什么？

sign.c中有以下代码

```
buf[510] = 0x55;
buf[511] = 0xAA;
FILE *ofp = fopen(argv[2], "wb+");
size = fwrite(buf, 1, 512, ofp);
if (size != 512) {
    fprintf(stderr, "write '%s' error, size is %d.\n", argv[2], size);
    return -1;
}
fclose(ofp);
printf("build 512 bytes boot sector: '%s' success!\n", argv[2]);
```
说明size=512，且510处为0x55,511处为0xAA


##练习2：使用qemu执行并调试lab1中的软件。（要求在报告中简要写出练习过程）

###为了熟悉使用qemu和gdb进行的调试工作，我们进行如下的小练习：

###从CPU加电后执行的第一条指令开始，单步跟踪BIOS的执行。

更改tools/gdbinit为
```
set architecture i8086
target remote :1234
```
意思是设置调试的CPU为8086
然后设置通信端口

之后开始debug
使用si命令可以执行一行汇编代码
而使用x/i $pc命令，可以查看当前对应的汇编代码
而x/2i x/3i可以多查看后面几行

```
7: x/i $pc  
=> 0xb7fec1ec:    mov    %eax,%edi  
6: x/2i $pc  
=> 0xb7fec1ec:    mov    %eax,%edi  
   0xb7fec1ee:    shr    $0x8,%edi  
5: x/3i $pc  
=> 0xb7fec1ec:    mov    %eax,%edi  
   0xb7fec1ee:    shr    $0x8,%edi  
   0xb7fec1f1:    mov    %edi,%ecx  
```

###在初始化位置0x7c00设置实地址断点,测试断点正常。

```
set architecture i8086
target remote :1234
break *0x7c00
continue
x /i $pc
```

意思是在0x7c00处设置断点break
然后开始执行continue
最后显示当前代码x

###从0x7c00开始跟踪代码运行,将单步跟踪反汇编得到的代码与bootasm.S和 bootblock.asm进行比较。
通过x/8i命令，进行一步一步的跟踪可得
也可以更改8为其他数字，查看更多或更少代码
最后可以发现与bootasm.S和bootblock.asm的部分代码一致

###自己找一个bootloader或内核中的代码位置，设置断点并进行测试。

```
.code16                                             # Assemble for 16-bit mode
    cli                                             # Disable interrupts
    cld                                             # String operations increment

    # Set up the important data segment registers (DS, ES, SS).
    xorw %ax, %ax                                   # Segment number zero
    movw %ax, %ds                                   # -> Data Segment
    movw %ax, %es                                   # -> Extra Segment
    movw %ax, %ss                                   # -> Stack Segment

    # Enable A20:
    #  For backwards compatibility with the earliest PCs, physical
    #  address line 20 is tied low, so that addresses higher than
    #  1MB wrap around to zero by default. This code undoes this.

```
在之后查看0x7c00后的部分
使用i r命令，查看寄存器值

##练习3：分析bootloader进入保护模式的过程。（要求在报告中写出分析）

###BIOS将通过读取硬盘主引导扇区到内存，并转跳到对应内存中的位置执行bootloader。请分析bootloader是如何完成从实模式进入保护模式的。

一开始将cs=0，pc=0x7c00后
在code16处，将重要的段寄存器DS，ES，SS设置为ax，也就是0

```
seta20.1:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    testb $0x2, %al
    jnz seta20.1

    movb $0xd1, %al                                 # 0xd1 -> port 0x64
    outb %al, $0x64                                 # 0xd1 means: write data to 8042's P2 port

seta20.2:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    testb $0x2, %al
    jnz seta20.2

    movb $0xdf, %al                                 # 0xdf -> port 0x60
    outb %al, $0x60                                 # 0xdf = 11011111, means set P2's A20 bit(the 1 bit) to 1
```

之后设置8042的P2的A20端口为1，使用32条地址总线
```
lgdt gdtdesc
movl %cr0, %eax
orl $CR0_PE_ON, %eax
movl %eax, %cr0
```
初始化载入GDT表
然后修改cr0为CR0_PE_ON，说明进入了保护模式

```
ljmp $PROT_MODE_CSEG, $protcseg

protcseg:
    # Set up the protected-mode data segment registers
    movw $PROT_MODE_DSEG, %ax                       # Our data segment selector
    movw %ax, %ds                                   # -> DS: Data Segment
    movw %ax, %es                                   # -> ES: Extra Segment
    movw %ax, %fs                                   # -> FS
    movw %ax, %gs                                   # -> GS
    movw %ax, %ss                                   # -> SS: Stack Segment

    # Set up the stack pointer and call into C. The stack region is from 0--start(0x7c00)
    movl $0x0, %ebp
    movl $start, %esp
    call bootmain
```

设置寄存器和栈指针，栈的空间是0~start，这些操作是保护模式下的

##练习4：分析bootloader加载ELF格式的OS的过程。（要求在报告中写出分析）
###通过阅读bootmain.c，了解bootloader如何加载ELF文件。通过分析源代码和通过qemu来运行并调试bootloader&OS，

###bootloader如何读取硬盘扇区的？
```
/* readsect - read a single sector at @secno into @dst */
static void
readsect(void *dst, uint32_t secno) {
    // wait for disk to be ready
    waitdisk();

    outb(0x1F2, 1);                         // count = 1，读取扇区的数目是1
    outb(0x1F3, secno & 0xFF);
    outb(0x1F4, (secno >> 8) & 0xFF);
    outb(0x1F5, (secno >> 16) & 0xFF);
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
    /*
      目标的格式为secno的二进制码第4位置0，表示访问磁盘0
    */
    outb(0x1F7, 0x20);                      // cmd 0x20 - read sectors，读取扇区命令

    // wait for disk to be ready
    waitdisk();

    // read a sector
    insl(0x1F0, dst, SECTSIZE / 4);         // 读入到dst，同时单位转换到DW
}
```
readsect函数会从secno扇区来读取数据到dst，长度为SECSIZE
```
/* *
 * readseg - read @count bytes at @offset from kernel into virtual address @va,
 * might copy more than asked.
 * */
static void
readseg(uintptr_t va, uint32_t count, uint32_t offset) {
    uintptr_t end_va = va + count;

    // round down to sector boundary
    va -= offset % SECTSIZE;

    // translate from bytes to sectors; kernel starts at sector 1
    uint32_t secno = (offset / SECTSIZE) + 1;

    // If this is too slow, we could read lots of sectors at a time.
    // We'd write more to memory than asked, but it doesn't matter --
    // we load in increasing order.
    for (; va < end_va; va += SECTSIZE, secno ++) {
        readsect((void *)va, secno);
    }
}
```
而readseg使用了readsect函数，可以读入任意长度的内容，方法还是一块块读，使用for循环

###bootloader是如何加载ELF格式的OS？

```
/* bootmain - the entry of bootloader */
void
bootmain(void) {
    // read the 1st page off disk
    readseg((uintptr_t)ELFHDR, SECTSIZE * 8, 0);

    // is this a valid ELF?
    if (ELFHDR->e_magic != ELF_MAGIC) {
        goto bad;
    }

    struct proghdr *ph, *eph;

    // load each program segment (ignores ph flags)
    ph = (struct proghdr *)((uintptr_t)ELFHDR + ELFHDR->e_phoff);
    eph = ph + ELFHDR->e_phnum;
    for (; ph < eph; ph ++) {
        readseg(ph->p_va & 0xFFFFFF, ph->p_memsz, ph->p_offset);
    }

    // call the entry point from the ELF header
    // note: does not return
    ((void (*)(void))(ELFHDR->e_entry & 0xFFFFFF))();

bad:
    outw(0x8A00, 0x8A00);
    outw(0x8A00, 0x8E00);

    /* do nothing */
    while (1);
}
```
在这里，判断了是否在加载ELF文件，如果格式正确
那么分别加载每一段程序段

##练习5：实现函数调用堆栈跟踪函数 （需要编程）

###我们需要在lab1中完成kdebug.c中函数print_stackframe的实现，可以通过函数print_stackframe来跟踪函数调用堆栈中记录的返回地址。在如果能够正确实现此函数，可在lab1中执行 “make qemu”后，在qemu模拟器中得到类似如下的输出：
```
……
ebp:0x00007b28 eip:0x00100992 args:0x00010094 0x00010094 0x00007b58 0x00100096
    kern/debug/kdebug.c:305: print_stackframe+22
ebp:0x00007b38 eip:0x00100c79 args:0x00000000 0x00000000 0x00000000 0x00007ba8
    kern/debug/kmonitor.c:125: mon_backtrace+10
ebp:0x00007b58 eip:0x00100096 args:0x00000000 0x00007b80 0xffff0000 0x00007b84
    kern/init/init.c:48: grade_backtrace2+33
ebp:0x00007b78 eip:0x001000bf args:0x00000000 0xffff0000 0x00007ba4 0x00000029
    kern/init/init.c:53: grade_backtrace1+38
ebp:0x00007b98 eip:0x001000dd args:0x00000000 0x00100000 0xffff0000 0x0000001d
    kern/init/init.c:58: grade_backtrace0+23
ebp:0x00007bb8 eip:0x00100102 args:0x0010353c 0x00103520 0x00001308 0x00000000
    kern/init/init.c:63: grade_backtrace+34
ebp:0x00007be8 eip:0x00100059 args:0x00000000 0x00000000 0x00000000 0x00007c53
    kern/init/init.c:28: kern_init+88
ebp:0x00007bf8 eip:0x00007d73 args:0xc031fcfa 0xc08ed88e 0x64e4d08e 0xfa7502a8
<unknow>: -- 0x00007d72 –
……
```
###请完成实验，看看输出是否与上述显示大致一致，并解释最后一行各个数值的含义。

最后一行的含义依次为程序的ebp值，eip值以及调用时传入的参数
```
# Set up the stack pointer and call into C. The stack region is from 0--start(0x7c00)
movl $0x0, %ebp
movl $start, %esp
call bootmain
```
ebp从0x7c00开始，之后又调用了call bootmain
所以这个bootmain函数的ebp值为0x7bf8

##练习6：完善中断初始化和处理 （需要编程）
###请完成编码工作和回答如下问题：

###中断描述符表（也可简称为保护模式下的中断向量表）中一个表项占多少字节？其中哪几位代表中断处理代码的入口？
一个表项是8 bytes，其中0~1和6~7字节形成位移，2~3字节是selector，共同组成中断处理代码入口
###请编程完善kern/trap/trap.c中对中断向量表进行初始化的函数idt_init。在idt_init函数中，依次对所有中断入口进行初始化。使用mmu.h中的SETGATE宏，填充idt数组内容。每个中断的入口由tools/vectors.c生成，使用trap.c中声明的vectors数组即可。

###请编程完善trap.c中的中断处理函数trap，在对时钟中断进行处理的部分填写trap函数中处理时钟中断的部分，使操作系统每遇到100次时钟中断后，调用print_ticks子程序，向屏幕上打印一行文字”100 ticks”。

###在v9-cpu中如何实现时钟中断的；
时钟中断使用timeout和timer变量实现，一开始都为0,timer在循环一次后+delta，而timeout在TIME操作后会被设置为a寄存器中的值。当timer超过timeout后，跳转到interrupt的位置，同时把timer清零。

###v9-cpu指令，关键变量描述有误或不全的情况；
a: a寄存器
b: b寄存器
c: c寄存器
f: f浮点寄存器
g: g浮点寄存器
ir:　指令寄存器
xpc: pc在host内存中的值
fpc: pc在host内存中所在页的下一页的起始地址值
tpc: pc在host内存中所在页的起始地址值
xsp: sp在host内存中的值
tsp: sp在host内存中所在页的起始地址值
fsp: 辅助判断是否要经过tr/tw的分析
ssp: kernel态的栈指针
usp: user态的栈指针
cycle: 周期计数
xcycle: 与xpc相关的计数，最后的实际周期数为cycle+(xpc-xcycle)/4
timer: 计时器
timeout: 应当执行时钟中断的时间
detla: 一个时钟周期时长

###在v9-cpu中的跳转相关操作是如何实现的；
对于JMP和JMPI直接修改了xpc的值
对于JSR和JSRA需要保存当前的pc值到栈中，然后在修改xpc的值
对于很多的BRANCH指令，分别判断是否符合条件，如果符合就跳转

###在v9-cpu中如何设计相应指令，可有效实现函数调用与返回；
使用JSR或者JSRA跳转进入函数，之后从栈中返回时，从栈中拿出栈顶就是应当返回的PC，POP后，使用JMP跳转到即可。
如果需要参数传递，那么应当先用PUSH指令将参数压入栈中，再使用JSRA或者JSR，在函数内需要调用参数时，使用栈中地址为SP+对应次序*4的元素即可。返回后将栈中上述加入的元素都POP出来。

###emhello/os0/os1等程序被加载到内存的哪个位置,其堆栈是如何设置的；
如代码中674行至680行所示，f为对应文件，read到的位置为mem+memmsz-FS_SZ这个位置，如果mem为分配内存的开头，那么read到的位置为结尾部分，这里读入filesystem。之后在688行，读入文件，读入到mem开头。
sp初始为memsz-FS_SZ，也就是分配的全部空间扣除filesystem后的末尾。堆栈指针sp在PUSH时减少，POP时增加，也就是说是栈指针的值随着栈的变大时从大往小变化的。

###在v9-cpu中如何完成一次内存地址的读写的；
    if (!(p = tr[a >> 12]) && !(p = rlook(a))) goto exception;
以MCHR操作为例，将a>>12取到对应的虚页号，然后判断是否在页表中，如果在直接短路然后进行操作。
如果不在，那么调用rlook函数，如果paging为0,那么对应的地址就是实际地址。否则，找到地址高10位对应的page directory entry，再找到地址中间10位对应的page table entry，这样就找到对应的实际地址了，之后使用setpage修改。
如果时写的操作，那么调用wlook函数，其中查询对应页面的方法和上面一样。注意使用的是dirty技术，在rlook时会保证dirty落实。

###在v9-cpu中如何实现分页机制；
分页的方法如上问所答，分为前10位，中间10位和最后12位页内偏移量。

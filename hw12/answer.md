(2):尝试在panic函数中获取并输出用户栈和内核栈的函数嵌套信息，然后在你希望的地方人为触发panic函数，并输出上述信息。

在trap_dispatch()里添加：

if (!trap_in_kernel(tf)) { panic("1"); } 来触发panic

在panic()里添加print_stackframe()来输出输出用户栈和内核栈的函数嵌套信息

(3):尝试在panic函数中获取和输出页表有效逻辑地址空间范围和在内存中的逻辑地址空间范围，然后在你希望的地方人为触发panic函数，并输出上述信息。

在trap_dispatch()里添加：

if (tf->tf_cs != KERNEL_CS) { panic("2"); }

在panic()里添加print_pgdir();来输出页表有效逻辑地址空间范围和在内存中的逻辑地址空间范围

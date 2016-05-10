# cvp->counnt

cvp->count 对应的就是cv.count，当然不可能会<0，因为每次在cond_wait中是先++，当这个函数结束时再--的，所以一定是>=0的；当然可能>1，当有多个进程先后调用wait时，cv.count>1。

但是！在实际的实现中是有BUG的，现在列出一种可能：

    A: cvp->count++ //cvp->count = 1
    A: cvp->count-- //被B打断，这时计算出了0，但是没有赋值给cvp->count，cvp->count仍为1
    B: cvp->count++ //cvp->count = 2
    A: cvp->count-- //返回被打断处，cvp->count被赋值为0
    B: cvp->count-- //cvp->count = -1

也就是说cvp->count 可能小于1的，因为对于++,--没有完全互斥

# cvp->owner->next_count

cv->owner->next_count对应的时mt.next_count，也就是由于发出signal而等待的线程总数。这个数值的改变在cond_signal中，同样也是先++，当函数退出时再--。


问题的关键在于是否会有两个进程同时cond_signal一个cvp，当然是有可能的，假设A wait，B，C同时进入cond_signal。

那么首先next_count可能会被+2次，当然就会>1了。
另一种可能是，B，C产生上述的++，--冲突，那么也可能会<0。

可是这样的原因在于用户对于管程的使用错误，因为这个时候B，C竟然能同时signal同一个cvp，说明对于mtp的管理出了问题，这样的BUG我认为应该归于不正常的使用。

# Hansen & Hoare

现在的实现是Hoare的，会在Signal之后立即执行wait的进程，另一种实现参见本目录下的代码

具体的方式就是在wait到了所需要的变量后，及时把mutex还给原来的进程，让signal的线程继续执行。

# BUG

有BUG，参见第一题和第二题中所述

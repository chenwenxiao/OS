# cvp->counnt

cvp->count 对应的就是cv.count，当然不可能会<0，因为每次在cond_wait中是先++，当这个函数结束时再--的，所以一定是>=0的；当然可能>1，当有多个进程先后调用wait时，cv.count>1。

# cvp->owner->next_count

cv->owner->next_count对应的时mt.next_count，也就是当前等待的线程总数。这个数值的改变在cond_signal中，同样也是先++，当函数退出时再--，那么和上面一题一样，数值上不可能<0，有可能会大于1。


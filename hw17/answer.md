### PRO1
根据sfs文件系统的状态变化信息，给出具体的文件相关操作内容.

步骤是：

1. 创建了一个g目录
2. 创建了q文件
3. 创建了u文件
4. 创建了指向u的硬链接x
5. 创建了目录t
6. 创建了文件g/c
7. 删除了x
8. 创建了目录g/w
9. 在c中写入o
10. 创建文件n

### PRO2

在sfs-homework.py 参考代码的基础上，实现 writeFile, createFile, createLink, deleteFile，使得你的实现能够达到与问题1的正确结果一致

见代码

### PRO3

实现soft link机制，并设计测试用例说明你实现的正确性。

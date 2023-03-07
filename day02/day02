March 4, 2023 

### 错误处理

[perror()函数的使用 - 茗若然 - 博客园](https://www.cnblogs.com/mingruoran/p/14924738.html)

> 当程序的当前函数（这类函数包括系统函数和库函数）出现错误，会将错误值保存在errno这个全局变量中，然后当程序执行到perror()函数的时候，会先打印参数s中的字符串，接着打印一个冒号，和errno值对应的错误描述字符串。

[exit(EXIT_FAILURE)_Numen_isnot_delay的博客-CSDN博客](https://blog.csdn.net/ICANITHINK/article/details/90798689)

> exit(0): 正常执行程序并退出程序。
>
>
> exit(1): 非正常执行导致退出程序。
>
> stdlib.h头文件中 定义了两个变量：
>
> #define EXIT_SUCCESS 0
> #define EXIT_FAILURE 1
>
> exit(EXIT_SUCCESS)  : 代表安全退出。
>
> exit(EXIT_FAILURE) ： 代表异常退出。

### 遇到的问题：无法使用read/write

```bash
PS D:\projects-my\CppServer-Win\cppServer-Win\day02> .\server
server WSAStartup success
new client fd: 284, IP: 127.0.0.1, port: 54882
socket read error: Bad file descriptor
```

💡将read/write改为recv/send之后可以运行

> recv() 和 send() 函数分别用于接受和发送数据。如果是在 linux 下，则分别为 read() 和 send() 函数 。

```bash
PS D:\projects-my\CppServer-Win\cppServer-Win\day02> .\server
server WSAStartup success
new client fd: 288, IP: 127.0.0.1, port: 54837
message from client fd 288: 123
message from client fd 288: 234
message from client fd 288: 345
message from client fd 288: 456
message from client fd 288: 789
message from client fd 288: 789
socket read error: No error
```

💡但在client中control+C之后，与教程报错不一样。

💡建议在Linux系统上再次试验。

💡一个比较好的教程：

[（初学者的福音）windows下实现socket通信(TCP/IP)代码详解——服务端篇](https://zhuanlan.zhihu.com/p/551218170)
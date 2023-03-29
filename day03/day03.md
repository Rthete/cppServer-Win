March 27, 2023 

学习并测试了Linux的epoll模型后，决定学习一下Windows的IOCP模型~

> 首先简单说一下 epoll 和 kqueue 这两个东西，其实相差的并不大，API 名称不一样，还有一些触发的行为细节不一样，但总的来说都是在缓冲区有内容时触发事件进行处理（epoll 的 ET 边缘触发模式），但 IOCP 是提交事件，事件处理完成后（读取或发送一定的字节数）才触发事件。
> 
1. **Windows IOCP模型**

> 完成端口可以抽象为一个公共消息队列，当用户请求到达时，完成端口把这些请求加入其抽象出的公共消息队列。这一过程与多个工作线程轮询消息队列并从中取出消息加以处理是并发操作。这种方式很好地实现了异步通信和负载均衡，因为它使几个线程“公平地”处理多客户端的I/O，并且线程空闲时会被挂起，不会占用CPU周期。
> 

> IOCP模型充分利用Windows系统内核，可以实现仅用少量的几个线程来处理和多个client之间的所有通信，消除了无谓的线程上下文切换，最大限度的提高了网络通信的性能。
> 

<aside>
💡 IOCP的使用主要分为以下几步：

</aside>

- 调用`CreateIoCompletionPort`函数，创建完成端口(iocp)对象
- 创建一个或多个工作线程，在完成端口上执行并处理投递到完成端口上的I/O请求
- Socket关联iocp对象，在Socket上投递网络事件
- 工作线程调用`GetQueuedCompletionStatus`函数获取完成通知封包，取得事件信息并进行处理

[Windows下的IOCP模型（一）：介绍与简单使用_Summer_night_star的博客-CSDN博客](https://blog.csdn.net/Summer_night_star/article/details/120803104)

[Windows 下 IOCP 的简单使用](https://zhuanlan.zhihu.com/p/544666070)

1. **WSA：Windows Sockets Asynchronous**

<aside>
💡 **socket与WSASocket的区别**

socket()函数创建一个通讯节点并返回一个套接字，如果是阻塞套接字时，相应api会阻塞。

WSASocket()的发送操作和接收操作都可以被重叠使用，接收函数可以被多次调用，发送函数也可以被多次调用，组成一个发送缓冲区队列，socket()却只能发过之后等待回消息才可做下一步操作。

</aside>

1. **多线程相关**

[C++ thread用法总结(整理)_顺其自然~的博客-CSDN博客](https://blog.csdn.net/fuhanghang/article/details/114818215)

### 实验

实现了使用IOCP（IO完成端口）接收事件，并使用了单个工作线程处理事件（read事件）。

首先使用`WSASocketW`函数创建套接字，并绑定地址：

```bash
# server
D:\Projects-my\cpp_study\cppServer\CppServer-Win\day03>.\iocp
WSASocketW: 172, IP: 127.0.0.1, port: 8080
```

初始化客户端，创建套接字并绑定地址：

```bash
# client
D:\Projects-my\cpp_study\cppServer\CppServer-Win\day03>.\client
server WSAStartup success
client socket 252 connect to 127.0.0.1, port: 8080
```

在客户端输入`hello`后程序退出，服务端初始化工作线程和接收进程。

接收进程将接收套接字绑定到IOCP，并提交一次读取事件；

工作线程获取队列中的事件，进行处理。

```bash
# server接收到"hello"
read bytes: 1024
hello
type any to quit...
```

最后键入任意字符退出，向所有工作线程发送退出指令，程序结束。

March 25, 2023 

## Linux epoll

《Linux高性能服务器编程》

1. epoll需要使用一个额外的文件描述符，来唯一标识内核中的这个事件表：

```cpp
#include＜sys/epoll.h＞
int epoll_create(int size)
// 该函数返回的文件描述符将用作其他所有epoll系统调用的第一个参数，以指定要访问的内核事件表。
```

[epoll_create1与epoll_create区别 - 李兆龙的博客 - 博客园](https://www.cnblogs.com/lizhaolong/p/16437375.html)

epoll监听事件的描述符会放在一颗红黑树上

[图解：什么是红黑树？](https://zhuanlan.zhihu.com/p/273829162)

1. 操作epoll的内核事件表

```cpp
#include＜sys/epoll.h＞
int epoll_ctl(int epfd,int op,int fd,struct epoll_event*event)
// epoll_ctl成功时返回0，失败则返回-1并设置errno。
```

epoll_event的定义如下：

```cpp
struct epoll_event
{
__uint32_t events;/*epoll事件*/
epoll_data_t data;/*用户数据*/
};
```

epoll_data_t是一个联合体，其4个成员中使用最多的是fd，它指定事件所从属的目标文件描述符。

[深入理解C语言之union（共用体）和结构体struct_union结构体_小灰俠的博客-CSDN博客](https://blog.csdn.net/weixin_41040351/article/details/118701770)

1. epoll_wait函数

```cpp
#include <sys/epoll.h>
epoll_wait(int epfd, struct epoll_event*events, int maxevents, int timeout)
// 成功时返回就绪的文件描述符的个数，失败时返回-1并设置errno。
```

> epoll_wait函数如果检测到事件，就将所有就绪的事件从内核事件表（由epfd参数指定）中复制到它的第二个参数events指向的数组中。这个数组只用于输出epoll_wait检测到的就绪事件，而不像select和poll的数组参数那样既用于传入用户注册的事件，又用于输出内核检测到的就绪事件。
> 
1. LT和ET

> epoll对文件描述符的操作有两种模式：LT（Level Trigger，电平触发）模式和ET（Edge Trigger，边沿触发）模式。
> 

LT模式是默认的工作模式，这种模式下epoll相当于一个效率较高的poll。

当往epoll内核事件表中注册一个文件描述符上的EPOLLET事件时，epoll将以ET模式来操作该文件描述符。ET模式是epoll的高效工作模式。

当epoll_wait检测到其上有事件发生并将此事件通知应用程序后，应用程序必须立即处理该事件，因为**后续的epoll_wait调用将不再向应用程序通知这一事件。**

### 实验

运行server，并另开两个终端，运行两个client，可以看到server端显示连接上了两个client fd：

```bash
zxy@xd:~/cppServer/30dayMakeCppServer-main/code/day03$ ./server
new client fd 5! IP: 127.0.0.1 Port: 54000
new client fd 6! IP: 127.0.0.1 Port: 38472
```

分别从客户端发送消息，可见服务器接收到两个客户端的消息：

```bash
# client1
1123
message from server: 1123
# client2
2123
message from server: 2123
# server
message from client fd 5: 1123
finish reading once, errno: 11
message from client fd 6: 2123
finish reading once, errno: 11
```

断开客户端1后，服务端显示

```bash
# client1
^C
# server
EOF, client fd 5 disconnected
```

此时客户端2依然与服务端连接，可以正常收发消息：

```bash
# client2
2234
message from server: 2234
# server
message from client fd 6: 2234
finish reading once, errno: 11
```
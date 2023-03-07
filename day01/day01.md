<!--
 * @FilePath: \cppServer-Win\day01\day01.md
 * @Description:  
 * @Author: rthete
 * @Date: 2023-03-07 22:56:13
 * @LastEditTime: 2023-03-07 22:56:27
-->
February 28, 2023 

### 首先尝试打印创建的socket

```cpp
// 打印一下创建的socket试试
SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
cout << sockfd << endl;
```

```bash
d:\zxy\Cpp_study\CppServer\CppServer-Win\day01>g++ server.cpp -o server
C:\Users\1\AppData\Local\Temp\ccfNKOGk.o:server.cpp:(.text+0x20): undefined reference to `__imp_socket'
collect2.exe: error: ld returned 1 exit status
```

💡需要在命令中链接库`wsock32`

[记录VSCode C++网络编程 编译失败出现 undefined reference to _imp_socket等等_undefined reference to `__imp_socket_普通网友的博客-CSDN博客](https://blog.csdn.net/zb_3Dmax/article/details/126616846)

```bash
d:\zxy\Cpp_study\CppServer\CppServer-Win\day01>g++ server.cpp -o server **-lwsock32**
d:\zxy\Cpp_study\CppServer\CppServer-Win\day01>.\server
18446744073709551615
```

### 实现完整的server/client代码实验

💡windows的socket编程与linux差异较大

💡若不进行WSAstartup，会有以下结果

```bash
d:\zxy\Cpp_study\CppServer\CppServer-Win\day01>.\server
new client fd: 18446744073709551615, IP: 0.0.0.0, port: 0
```

[Windows下的socket编程_windows socket编程_时·风·人的博客-CSDN博客](https://blog.csdn.net/qq_44722098/article/details/126001350)

[#pragma comment 用法_Authur_gyc的博客-CSDN博客_#pragma comment](https://blog.csdn.net/WHY995987477/article/details/105108383)

[windows下socket编程的原理与分步实现_Shanfenglan7的博客-CSDN博客](https://blog.csdn.net/qq_41874930/article/details/108070886)

[1. Windows网络编程(C++ Socket编程)_windows网络编程技术_风声中※倦容的博客-CSDN博客](https://blog.csdn.net/qq_44731954/article/details/113796530)

💡实现了完整功能如下，首先运行服务端，再另外打开一个终端运行客户端

```bash
# server
d:\zxy\Cpp_study\CppServer\CppServer-Win\day01>.\server
server WSAStartup success
new client fd: 248, IP: 127.0.0.1, port: 4854
server exit
```

```bash
# client
D:\zxy\Cpp_study\CppServer\CppServer-Win\day01>.\client
client WSAStartup success
client exit
```


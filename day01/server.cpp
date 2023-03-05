/*
 * @FilePath: \cppServer-Win\day01\server.cpp
 * @Description:  
 * @Author: rthete
 * @Date: 2023-02-28 15:56:08
 * @LastEditTime: 2023-03-02 17:26:39
 */

#include <iostream>
#include <winsock2.h>                   // 网络头文件，Windows socket第二版
#pragma comment (lib, "ws2_32.lib")     // 加载网络库，Windows socket第二版，32位
#define socklen_t int
using namespace std;


int main() {
    // WSAStartup是为了向操作系统说明，我们要用哪个库文件，
    // 让该库文件与当前的应用程序绑定，从而就可以调用该版本
    // 的socket的各种函数。
    WSADATA wsadata;
        
    // 校验版本
	if (0 == WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		cout << "server WSAStartup success" << endl;
	}
	else
	{
		cout << "server WSAStartup fail" << endl;
        WSACleanup();
        exit(1);
	}

    // 创建socket
    // 第一个参数：IP地址类型，AF_INET表示使用IPv4，如果使用IPv6请使用AF_INET6。
    // 第二个参数：数据传输方式，SOCK_STREAM表示流格式、面向连接，多用于TCP。SOCK_DGRAM表示数据报格式、无连接，多用于UDP。
    // 第三个参数：协议，0表示根据前面的两个参数自动推导协议类型。设置为IPPROTO_TCP和IPPTOTO_UDP，分别表示TCP和UDP。
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 设置服务端地址族、IP地址和端口
    SOCKADDR_IN serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    // 将socket地址与文件描述符绑定
    bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));

    // 监听socket端口
    // 最大监听队列长度SOMAXCONN: 0x7fffffff(128)
    listen(sockfd, SOMAXCONN);

    // 客户端地址
    SOCKADDR_IN clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);

    // 接受客户端连接
    // 需要写入客户端socket长度：clnt_addr_len
    SOCKET clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
    
    cout << "new client fd: " << clnt_sockfd << ", IP: " << inet_ntoa(clnt_addr.sin_addr)
        << ", port: " << ntohs(clnt_addr.sin_port) << endl;

    // 关闭socket
    closesocket(sockfd);
    // 释放资源
    WSACleanup();

    cout << "server exit" << endl;

    return 0;
}

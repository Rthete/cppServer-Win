/*
 * @FilePath: \CppServer-Win\day01\client.cpp
 * @Description:  
 * @Author: rthete
 * @Date: 2023-02-28 18:20:41
 * @LastEditTime: 2023-02-28 19:54:43
 */

#include <iostream>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")
using namespace std;

int main() {
    WSADATA wsadata;
	if (0 == WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		cout << "client WSAStartup success" << endl;
	}
	else
	{
		cout << "client WSAStartup fail" << endl;
        WSACleanup();
        exit(1);
	}

    // 创建socket
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 设置服务端地址
    SOCKADDR_IN serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    // 尝试连接服务端
    connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));

    // 关闭socket
    closesocket(sockfd);
    // 释放资源
    WSACleanup();

    cout << "client exit" << endl;

    return 0;
}
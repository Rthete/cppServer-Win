/*
 * @FilePath: \cppServer-Win\day02\server.cpp
 * @Description:  
 * @Author: rthete
 * @Date: 2023-03-04 21:44:32
 * @LastEditTime: 2023-03-05 12:01:08
 */

#include <iostream>
#include <winsock2.h>
#include <unistd.h>
#include "util.h"
#define socklen_t int
using namespace std;

int main() {
    WSADATA wsadata;
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

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error"); 

    SOCKADDR_IN serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");
    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    SOCKADDR_IN clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);

    SOCKET clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
    errif(clnt_sockfd == -1, "socket accept error");

    cout << "new client fd: " << clnt_sockfd << ", IP: " << inet_ntoa(clnt_addr.sin_addr)
        << ", port: " << ntohs(clnt_addr.sin_port) << endl;

    while(true) {
        // 定义缓冲区
        char buf[1024];
        // 从客户端socket读取到缓冲区，返回已读数据大小
        ssize_t read_bytes = recv(clnt_sockfd, buf, sizeof(buf), 0);
        if(read_bytes > 0) {
            cout << "message from client fd " << clnt_sockfd << ": " << buf << endl;
            // 将相同数据写回到客户端
            send(clnt_sockfd, buf, sizeof(buf), 0);
        } else if(read_bytes == 0) {
            cout << "client fd " << clnt_sockfd << " disconnected" << endl;
            closesocket(clnt_sockfd);
            break;
        } else if(read_bytes == -1) {
            closesocket(clnt_sockfd);
            errif(true, "socket read error");
            break;
        }
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
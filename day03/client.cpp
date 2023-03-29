/*
 * @FilePath: \CppServer-Win\day03\client.cpp
 * @Description:  send one message
 * @Author: rthete
 * @Date: 2023-03-29 14:47:53
 * @LastEditTime: 2023-03-29 15:39:05
 */

#include <iostream>
#include <winsock2.h>
#include <unistd.h>
#include "util.h"
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
    errif(sockfd == -1, "socket create error", sockfd);

    SOCKADDR_IN serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8080);

    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error", sockfd);

    cout << "client socket " << sockfd << " connect to " << inet_ntoa(serv_addr.sin_addr)
        << ", port: " << ntohs(serv_addr.sin_port) << endl;
    
    char buf[1024];
    cin >> buf;
    ssize_t write_bytes = send(sockfd, buf, sizeof(buf), 0);
    if(write_bytes == -1) {
        cout << "socket already disconnected, can't write any more!" << endl;
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}

/*
 * @FilePath: \cppServer-Win\day02\client.cpp
 * @Description:  
 * @Author: rthete
 * @Date: 2023-03-04 22:25:51
 * @LastEditTime: 2023-03-05 23:25:00
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
    errif(sockfd == -1, "socket create error");

    SOCKADDR_IN serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");
    
    while(true) {
        char buf[1024];
        cin >> buf;

        ssize_t write_bytes = send(sockfd, buf, sizeof(buf), 0);
        if(write_bytes == -1) {
            cout << "socket already disconnected, can't write any more!" << endl;
            break;
        }

        ssize_t read_bytes = recv(sockfd, buf, sizeof(buf), 0);
        if(read_bytes > 0) {
            cout << "message from server: " << buf << endl;
        } else if(read_bytes == 0) {
            cout << "server socket disconnected" << endl;
            break;
        } else if(read_bytes == -1) {
            closesocket(sockfd);
            errif(true, "socket read error");
        }
    }
    closesocket(sockfd);
    WSACleanup();
    return 0;
}

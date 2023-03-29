/*
 * @FilePath: \CppServer-Win\day03\util.cpp
 * @Description:  
 * @Author: rthete
 * @Date: 2023-03-27 16:25:10
 * @LastEditTime: 2023-03-27 16:30:08
 */
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "util.h"
#include <stdio.h>
#include <stdlib.h> // exit()

void errif(bool condition, const char *errmsg, SOCKET serverSocket) {
    if(condition) {
        perror(errmsg);     // 打印errno的实际意义与errmsg
        closesocket(serverSocket);
        exit(EXIT_FAILURE); // 异常退出，等价于exit(1)
    }
}
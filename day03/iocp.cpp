/*
 * @FilePath: \CppServer-Win\day03\iocp.cpp
 * @Description:  
 * @Author: rthete
 * @Date: 2023-03-27 15:58:49
 * @LastEditTime: 2023-03-29 15:30:46
 */
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>
#include <atomic>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

#include "util.h"

using namespace std;

constexpr static size_t MaxBufferSize = 1024 * 1;
constexpr static size_t NumberOfThreads = 1;

HANDLE hIOCP = INVALID_HANDLE_VALUE;
SOCKET serverSocket = INVALID_SOCKET;
std::vector<std::thread> threadGroup;
std::atomic_bool isShutdown{false};

void AcceptWorkerThread();
void EventWorkerThread();

/**
 * 枚举 IO 操作的类型
 * 实际上在本例中我们只处理 Read 事件
*/
enum class IOType {
    Read,
    Write
};

struct IOContext {
    OVERLAPPED overlapped{};                // 重叠结构
    WSABUF wsaBuf{MaxBufferSize, buffer};   // 用于描述用户空间的缓冲区
    CHAR buffer[MaxBufferSize]{};           // 缓冲区
    IOType type{};                          // IO 操作的类型
    SOCKET socket = INVALID_SOCKET;         // 触发事件的套接字
    DWORD nBytes = 0;                       // 实际传输的字节数
};

int main() {
    WSADATA data{};
    WSAStartup(MAKEWORD(2, 2), &data);

    // 设置地址
    struct sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(8080);

    // 初始化socket
    unsigned long ul = 1;
    // 因为IOCP需要使用到重叠IO，所以创建监听套接字需要使用特殊的创建函数WSASocketW
    // dwFlags需要设置为WSA_FLAG_OVERLAPPED以表示启用重叠IO
    serverSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
    errif(INVALID_SOCKET == serverSocket,
        "FAILED TO CREATE SERVER SOCKET", serverSocket);
    // 设置非阻塞IO
    errif(SOCKET_ERROR == ioctlsocket(serverSocket, FIONBIO, &ul),
        "FAILED TO SET NONBLOCKING SOCKET", serverSocket);
    errif(SOCKET_ERROR == bind(serverSocket, (const struct sockaddr *) &address, sizeof(address)), 
        "FAILED TO BIND ADDRESS", serverSocket);
    errif(SOCKET_ERROR == listen(serverSocket, SOMAXCONN),
        "FAILED TO LISTEN SOCKET", serverSocket);

    cout << "WSASocketW: " << serverSocket << ", IP: " << inet_ntoa(address.sin_addr)
        << ", port: " << ntohs(address.sin_port) << endl;
    
    // 创建IOCP内核对象，并返回其句柄
    hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, NumberOfThreads);
    errif(INVALID_HANDLE_VALUE == hIOCP, "FAILED TO CREATE IOCP HANDLE", serverSocket);

    // 初始化工作线程
    for(size_t i = 0; i < NumberOfThreads; i++) {
        threadGroup.emplace_back(std::thread(EventWorkerThread));
    }

    void *lpCompletionKey = nullptr;

    // 初始化接收线程
    auto acceptThread = std::thread(AcceptWorkerThread);
    
    // 按任意键进入退出程序
    getchar();

    // 设置AccpetWorkerThread的退出标识符
    isShutdown = true;

    // 向所有工作线程发送退出指令
    for(size_t i = 0; i < NumberOfThreads; i++) {
        // 向工作线程发送一个事件以供处理
        // 此处我们将使用第二参数，将其设定为-1，仅供工作线程作为退出标识使用。
        PostQueuedCompletionStatus(hIOCP, -1, (ULONG_PTR)lpCompletionKey, nullptr);
    }

    // 确保工作线程在主线程退出前退出
    acceptThread.join();
    for(auto &thread: threadGroup) {
        thread.join();
    }

    WSACleanup();
    return 0;
}

/**
 * 接入线程
*/
void AcceptWorkerThread() {
    while(!isShutdown) {
        // 开始监听
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if(INVALID_SOCKET == clientSocket) continue;

        // 将接入的套接字也设置为非阻塞
        unsigned long ul = 1;
        if(SOCKET_ERROR == ioctlsocket(clientSocket, FIONBIO, &ul)) {
            shutdown(clientSocket, SD_BOTH);
            closesocket(clientSocket);
            continue;
        }

        // 将套接字绑定到现有的IOCP上
        if(nullptr == CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, 0, 0)) {
            shutdown(clientSocket, SD_BOTH);
            closesocket(clientSocket);
            continue;
        }

         DWORD nBytes = MaxBufferSize;
         DWORD dwFlags = 0;

         auto ioContext = new IOContext;
         ioContext->socket = clientSocket;
         ioContext->type = IOType::Read;

         // 提交一次读取事件
         auto rt = WSARecv(clientSocket, &ioContext->wsaBuf, 1, &nBytes, &dwFlags, &ioContext->overlapped, nullptr);
         
         auto err = WSAGetLastError();
         if(SOCKET_ERROR == rt && ERROR_IO_PENDING != err) {
            shutdown(clientSocket, SD_BOTH);
            closesocket(clientSocket);
            delete ioContext;
         }
    }
}

void EventWorkerThread() {
    IOContext *ioContext = nullptr;
    DWORD lpNumberOfBytesTransferred = 0;
    void *lpCompletionKey = nullptr;

    DWORD dwFlags = 0;
    DWORD nBytes = MaxBufferSize;

    // 循环获取完成事件
    while(true) {
        // 该函数用于接受PostQueuedCompletionStatus、WSARecv、WSASend所Post的事件请求
        BOOL bRt = GetQueuedCompletionStatus(
            hIOCP,
            &lpNumberOfBytesTransferred,
            (PULONG_PTR) &lpCompletionKey,
            (LPOVERLAPPED *) &ioContext,
            INFINITE
        );

        if(!bRt) continue;

        // cout << "lpNumberOfBytesTransferred = " << lpNumberOfBytesTransferred << endl;
        // 读取时：lpNumberOfBytesTransferred = 1024 即MaxBufferSize
        // 退出时：lpNumberOfBytesTransferred = 4294967295 即-1

        // 收到PostQueuedCompletionStatus发出的退出指令，停止循环
        if(lpNumberOfBytesTransferred == -1) break;

        if(lpNumberOfBytesTransferred == 0) continue;

        // 读到的字节总数
        ioContext->nBytes = lpNumberOfBytesTransferred;
        cout << "read bytes: " << ioContext->nBytes << endl;

        // 处理事件，只实现了read事件
        switch (ioContext->type) {
            case IOType::Read: {
                int nRt = WSARecv(
                    ioContext->socket,
                    &ioContext->wsaBuf,
                    1,
                    &nBytes,
                    &dwFlags,
                    &(ioContext->overlapped),
                    nullptr
                );
                auto e = WSAGetLastError();
                if(SOCKET_ERROR == nRt && e != WSAGetLastError()) {
                    closesocket(ioContext->socket);
                    delete ioContext;
                    ioContext = nullptr;
                } else {
                    setbuf(stdout, nullptr);
                    puts(ioContext->buffer);
                    fflush(stdout);
                    closesocket(ioContext->socket);
                    delete ioContext;
                    ioContext = nullptr;
                    
                    cout << "type any to quit..." << endl;
                }
                break;
            }
            case IOType::Write: {
                break;
            }
        }
        
    }
}
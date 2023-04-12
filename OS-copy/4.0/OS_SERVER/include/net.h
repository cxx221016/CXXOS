#pragma once
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <exception>
#include "Mem.h"

class Net
{
private:
    std::shared_ptr<Mem> mem;
    WSADATA wsaData;
    SOCKET s_server, s_accept;
    SOCKADDR_IN server_addr, accept_addr;
    char sendbuf[1024];
    char recvbuf[1024];
public:
    Net(std::shared_ptr<Mem> mem,const std::string& ip="127.0.0.1", int port=1234);
    ~Net();
    void run();
    bool chat();
};

// Path: OS\src\net.cpp
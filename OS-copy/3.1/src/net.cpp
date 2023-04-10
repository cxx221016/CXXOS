#include "../include/net.h"

Net::Net(std::shared_ptr<Mem> mem, const std::string &ip, int port)
{
    this->mem = mem;
    WORD version = MAKEWORD(2, 2);
    WSAStartup(version, &wsaData);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(port);

    s_server = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        std::cout << "connect error" << std::endl;
        WSACleanup();
    }
    else
        std::cout << "connect success" << std::endl;
}

Net::~Net()
{
    closesocket(s_server);
    WSACleanup();
}

bool Net::chat()
{
    std::cout << "please input the message: ";
    std::cin.getline(sendbuf, 1024);

    std::string tmp = sendbuf;
    std::stringstream ss(tmp);
    std::string cmd, path, name;
    ss >> cmd >> path >> name;
    if (cmd == "upload")
    {
        auto file = mem->findFile(path, name);
        if (file == nullptr)
        {
            std::cout << "can not find the file" << std::endl;
            return false;
        }
        std::stringstream ss;
        std::string innerfile = ss.str();
        send(s_server, sendbuf, 1024, 0);
        recv(s_server, recvbuf, 1024, 0);
        std::string get = recvbuf;
        if (get == "yes")
        {
            std::cout << "begin upload" << std::endl;
            for (int i = 0; i < innerfile.size(); ++i)
            {
                sendbuf[i] = innerfile[i];
            }
            sendbuf[innerfile.size()] = '\0';
            send(s_server, sendbuf, 1024, 0);
            recv(s_server, recvbuf, 1024, 0);
            std::cout << "end upload" << std::endl;
            strcpy(sendbuf, "end");
            send(s_server, sendbuf, 1024, 0);
            recv(s_server, recvbuf, 1024, 0);
            std::cout << recvbuf << std::endl;
        }
        else
        {
            std::cout << "can not upload" << std::endl;
            return false;
        }
    }
    else if (cmd == "download")
    {
        send(s_server, sendbuf, 1024, 0);
        recv(s_server, recvbuf, 1024, 0);
        std::string get = recvbuf;
        if (get == "no this file")
        {
            std::cout << recvbuf << std::endl;
            return false;
        }
        if (get != "yes")
        {
            std::cout << "can not download" << std::endl;
            return false;
        }
        auto dir = mem->findDir(path);
        if (dir == nullptr)
        {
            std::cout << "can not find the dir" << std::endl;
            return false;
        }
        std::cout << "begin download" << std::endl;
        recv(s_server, recvbuf, 1024, 0);
        std::string tmp = recvbuf;
        if (tmp == "end")
        {
            std::cout << "end download" << std::endl;
        }
        std::string innerfile = recvbuf;
        std::stringstream ss(innerfile);
        File newfile;
        ss >> newfile;
        auto clonecur = mem->current;
        mem->current = dir;
        mem->type(newfile.name, newfile.len, newfile.data);
        mem->current = clonecur;
        strcpy(sendbuf, "yes");
        send(s_server, sendbuf, 1024, 0);
    }
    else
    {
        send(s_server, sendbuf, 1024, 0);
        recv(s_server, recvbuf, 1024, 0);
        std::cout << "the message of server: " << recvbuf << std::endl;
    }
}
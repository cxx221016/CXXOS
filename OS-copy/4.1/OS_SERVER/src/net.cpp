#include "../include/net.h"

Net::Net(std::shared_ptr<Mem> mem, const std::string &ip, int port)
{
    this->mem = mem;
    WORD version = MAKEWORD(2, 2);
    WSAStartup(version, &wsaData);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(1234);

    s_server = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        std::cout << "bind failed" << std::endl;
        WSACleanup();
        throw std::runtime_error("bind error");
    }
    else
        std::cout << "bind success" << std::endl;

    if (listen(s_server, SOMAXCONN) < 0)
    {
        std::cout << "listen failed" << std::endl;
        WSACleanup();
        throw std::runtime_error("bind error");
    }
    else
        std::cout << "listen success" << std::endl;

    std::cout << "try link now ,wait a moment" << std::endl;

    int len = sizeof(SOCKADDR);
    s_accept = accept(s_server, (SOCKADDR *)&accept_addr, &len);
    if (s_accept == SOCKET_ERROR)
    {
        std::cout << "link failed" << std::endl;
        WSACleanup();
        throw std::runtime_error("link error");
    }
    std::cout << "link success" << std::endl;
}

Net::~Net()
{
    closesocket(s_server);
    closesocket(s_accept);
    WSACleanup();
}

void Net::run()
{
    std::cout << "net run\n"
              << std::endl;
    while (true)
    {
        if (chat())
            break;
    }
    std::cout << "net end\n"
              << std::endl;
}

bool Net::chat()
{
    recv(s_accept, recvbuf, 1024, 0);
    std::string tmp = recvbuf;
    std::stringstream ss(tmp);
    std::string cmd, path, name;
    ss >> cmd >> path >> name;
    if (cmd == "upload")
    {
        auto dir = mem->findDir(path);
        if (dir == nullptr)
        {
            std::cout << "can not find the dir" << std::endl;
            return false;
        }
        std::string get;
        std::cout << "Ready? (yes)" << std::endl;
        std::cin >> sendbuf;
        send(s_accept, sendbuf, 1024, 0);
        if (strcmp(sendbuf, "yes") != 0)
        {
            return false;
        }
        std::cout << "begin upload" << std::endl;
        recv(s_accept, recvbuf, 1024, 0);
        get = recvbuf;
        if (get == "end")
        {
            std::cout << "end upload" << std::endl;
            strcpy(sendbuf, "file has been upload successfully");
            send(s_accept, sendbuf, 1024, 0);
            return false;
        }
        std::stringstream ss(get);
        File curfile;
        ss>>curfile;
        curfile.name.append("-copy");
        mem->addFile(path, curfile);
        strcpy(sendbuf, ".");
        send(s_accept, sendbuf, 1024, 0);
    }

    else if (cmd == "download")
    {
        auto file = mem->findFile(path, name);
        if (file == nullptr)
        {
            strcpy(sendbuf, "no this file");
            send(s_accept, sendbuf, 1024, 0);
            std::cout << sendbuf << " " << path << std::endl;
            return false;
        }
        std::stringstream ss;
        ss << *file;
        std::string innerfile=ss.str();
        //std::cout<<innerfile<<std::endl;
        std::cout<<"Ready? (yes)"<<std::endl;
        std::cin>>sendbuf;
        send(s_accept, sendbuf, 1024, 0);
        if (strcmp(sendbuf, "yes") != 0)
        {
            return false;
        }
        recv(s_accept, recvbuf, 1024, 0);
        std::cout << "begin download" << std::endl;
        for(int i=0;i<innerfile.size();++i)
        {
            sendbuf[i]=innerfile[i];
        }
        send(s_accept, sendbuf, 1024, 0);
        recv(s_accept, recvbuf, 1024, 0);
        std::cout << "end download" << std::endl;
        send(s_accept, sendbuf, 1024, 0);
    }
    else if(cmd=="~net")
    {
        std::cout<<"net end"<<std::endl;
        return true;
    }
    else
    {
        std::cout << "message from client: " << recvbuf << std::endl;
        std::cout << "please input the reply: ";
        std::cin >> sendbuf;
        send(s_accept, sendbuf, 1024, 0);
    }
    return false;
}

#include "../include/net.h"

Net::Net(std::shared_ptr<Mem> mem, const std::string &ip, int port)
{
    this->mem = mem;
    this->flag=true;
    WORD version = MAKEWORD(2, 2);
    WSAStartup(version, &wsaData);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(port);

    s_server = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        std::cout << "connect error" << std::endl;
        throw std::runtime_error("connect error");
        //WSACleanup();
    }
    else
        std::cout << "connect success" << std::endl;
}

Net::~Net()
{
    closesocket(s_server);
    WSACleanup();
}

void Net::run()
{
    std::cout<<"net run\n"<<std::endl;
    while (true)
    {
        if (chat())   break;
    }
    std::cout<<"net end\n"<<std::endl;
}

bool Net::chat()
{
    std::cout << "please input the message: ";
    std::string cmd, path, name;
    if(flag)
    {
        std::cin>>sendbuf;
        cmd=sendbuf;
        flag=false;
    } 
    else
    {
        std::cin.getline(sendbuf,1024);
        std::stringstream ss(sendbuf);
        ss>>cmd>>path>>name;
    }
    
     
    
    //std::cin.ignore(100,'\n');
    /*
    std::string cmd, path, name;
    std::cin>>cmd>>path>>name;
    std::string tmp = cmd + " " + path + " " + name;
    memset(sendbuf, 0, 1024);
    for(int i=0;i<tmp.size();++i)
    {
        sendbuf[i] = tmp[i];
    }*/
    if (cmd == "upload")
    {
        auto file = mem->findFile(path, name);
        if (file == nullptr)
        {
            std::cout << "can not find the file" << std::endl;
            return false;
        }
        std::stringstream ss;
        ss<<*file;
        std::string innerfile = ss.str();
        send(s_server, sendbuf, 1024, 0);
        recv(s_server, recvbuf, 1024, 0);
        std::string get = recvbuf;
        if (get == "yes")
        {
            std::cout << "begin upload" << std::endl;
            memset(sendbuf, 0, 1024);
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
        send(s_server, sendbuf, 1024, 0);
        std::cout << "begin download" << std::endl;
        recv(s_server, recvbuf, 1024, 0);
        std::string tmp = recvbuf;
        std::string innerfile = recvbuf;
        std::stringstream ss(innerfile);
        File newfile;
        ss >> newfile;
        newfile.name.append("-copy");
        mem->addFile(path,newfile);
        strcpy(sendbuf, "yes");
        send(s_server, sendbuf, 1024, 0);
        std::cout<<"end download"<<std::endl;
        recv(s_server, recvbuf, 1024, 0);
    }
    else if(cmd =="~net")
    {
        send(s_server, sendbuf, 1024, 0);
        return true;
    }
    else
    {
        send(s_server, sendbuf, 1024, 0);
        recv(s_server, recvbuf, 1024, 0);
        std::cout << "the message of server: " << recvbuf << std::endl;
    }
    return false;

}
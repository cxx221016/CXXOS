#include "include/Mem.h"
#include "include/threadpool.h"
#include "include/Func.h"

/*
cmake -G "MinGW Makefiles" ..
*/
Mem mem;
threadpool pool(1);
std::priority_queue<std::pair<std::shared_future<int>, Attr>, std::vector<std::pair<std::shared_future<int>, Attr>>, OutComp> ress;
bool command(std::string &cmd);

int main()
{
    std::string cmd;
    std::cout << "kernel 1.0.0\n";
    std::cout << "cxxos 1.0.0\n";
    std::cout << "kernel loaded\n";
    std::cout << "Memory size: " << mem.getcapcity() << "KB\n";
    std::cout << "Sche loaded\n";
    std::cout << "Welcome to the file system simulator.\n";
    std::cout << "Type 'help' for more information.\n";
    std::cout << "cxx@CXX:/CXX$: ";
    while(std::cin>>cmd)
    {
        bool flag=command(cmd);
        if(!flag) break;
    }
    /*
    while (std::cin >> cmd)
    {
        // file
        if (cmd == "md")
        {
            std::string name;
            std::cin >> name;
            mem.md(name);
        }
        else if (cmd == "rd")
        {
            std::string name;
            std::cin >> name;
            mem.rd(name);
        }
        else if (cmd == "type")
        {
            std::string name;
            std::cin >> name;
            int len;
            std::cin >> len;
            std::string data;
            std::cin >> data;
            mem.type(name, len, data);
        }
        else if (cmd == "del")
        {
            std::string name;
            std::cin >> name;
            mem.del(name);
        }
        else if (cmd == "cd")
        {
            std::string name;
            std::cin >> name;
            mem.cd(name);
        }
        // Mem
        else if (cmd == "compact")
        {
            mem.compact();
        }
        else if (cmd == "status")
        {
            mem.status();
        }
        // Task
        else if (cmd == "submit")
        {

            unsigned int priority;
            unsigned int burst;
            std::string name;
            int arg0, arg1;
            std::cin >> priority >> burst >> name >> arg0 >> arg1;
            std::shared_ptr<Attr> attr = std::make_shared<Attr>(name, priority, burst);
            auto res = pool.submit(attr, opmap[name], arg0, arg1);
            ress.push(std::make_pair(res, *attr));
        }
        else if (cmd == "run")
        {
            pool.run();
            // std::pair<std::future<int>,Attr> res;
            while (!ress.empty())
            {
                auto res = ress.top();
                ress.pop();
                // res.first.get();
                // std::cout<<"Task "<<res.second.pid<<" "<<res.second.name<<" "<<res.first.get()<<"\n";
                printf("  res: %d\n", res.first.get());
            }
        }
        else if (cmd == "dir")
        {
            mem.dir();
        }
        else if (cmd == "exit")
        {
            break;
        }

        else if (cmd == "debug")
        {
            mem.debug();
        }
        else if (cmd == "help")
        {
            std::cout << "md <name> : make directory\n";
            std::cout << "rd <name> : remove directory\n";
            std::cout << "type <name> <len> <data> : type file\n";
            std::cout << "del <name> : delete file\n";
            std::cout << "cd <name> : change directory\n";
            std::cout << "dir : list directory\n";
            std::cout << "exit : exit\n";
            std::cout << "compact : compact\n";
            std::cout << "status : status\n";
            std::cout << "submit <priority> <burst> <name> <arg0> <arg1> : submit task\n";
            std::cout << "run : run\n";
            std::cout << "debug : debug\n";
            std::cout << "help : help\n";
        }
        else
        {
            std::cerr << "Error: Invalid command.\n";
        }

        // std::cout<<"cxx@CXX:/CXX$: ";
        std::string temp;
        temp = std::string("cxx@CXX:") + mem.getcurdir() + std::string("$: ");
        std::cout << temp;
    }
    */
    return 0;
}

bool command(std::string &cmd)
{
    if(cmd=="&&") 
    {
        std::cout<<'\n';
        return true;
    }
    //file
    if (cmd == "md")
    {
        std::string name;
        std::cin >> name;
        mem.md(name);
    }
    else if (cmd == "rd")
    {
        std::string name;
        std::cin >> name;
        mem.rd(name);
    }
    else if (cmd == "type")
    {
        std::string name;
        std::cin >> name;
        int len;
        std::cin >> len;
        std::string data;
        std::cin >> data;
        mem.type(name, len, data);
    }
    else if (cmd == "del")
    {
        std::string name;
        std::cin >> name;
        mem.del(name);
    }
    else if (cmd == "cd")
    {
        std::string name;
        std::cin >> name;
        mem.cd(name);
    }
    // Mem
    else if (cmd == "compact")
    {
        mem.compact();
    }
    else if (cmd == "status")
    {
        mem.status();
    }
    // Task
    else if (cmd == "submit")
    {

        unsigned int priority;
        unsigned int burst;
        std::string name;
        int arg0, arg1;
        std::cin >> priority >> burst >> name >> arg0 >> arg1;
        std::shared_ptr<Attr> attr = std::make_shared<Attr>(name, priority, burst);
        auto res = pool.submit(attr, opmap[name], arg0, arg1);
        ress.push(std::make_pair(res, *attr));
    }
    else if (cmd == "run")
    {
        pool.run();
        // std::pair<std::future<int>,Attr> res;
        while (!ress.empty())
        {
            auto res = ress.top();
            ress.pop();
            // res.first.get();
            // std::cout<<"Task "<<res.second.pid<<" "<<res.second.name<<" "<<res.first.get()<<"\n";
            printf("  res: %d\n", res.first.get());
        }
    }
    else if (cmd == "dir")
    {
        mem.dir();
    }
    else if (cmd == "exit")
    {
        return false;
    }

    else if (cmd == "debug")
    {
        mem.debug();
    }
    else if (cmd == "help")
    {
        std::cout << "md <name> : make directory\n";
        std::cout << "rd <name> : remove directory\n";
        std::cout << "type <name> <len> <data> : type file\n";
        std::cout << "del <name> : delete file\n";
        std::cout << "cd <name> : change directory\n";
        std::cout << "dir : list directory\n";
        std::cout << "exit : exit\n";
        std::cout << "compact : compact\n";
        std::cout << "status : status\n";
        std::cout << "submit <priority> <burst> <name> <arg0> <arg1> : submit task\n";
        std::cout << "run : run\n";
        std::cout << "debug : debug\n";
        std::cout << "help : help\n";
    }
    else
    {
        std::cerr << "Error: Invalid command.\n";
    }

    // std::cout<<"cxx@CXX:/CXX$: ";
    std::string temp;
    temp = std::string("cxx@CXX:") + mem.getcurdir() + std::string("$: ");
    std::cout << temp;
    return true;
}
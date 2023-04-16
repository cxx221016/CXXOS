#include "Mem.h"
#include "threadpool.h"
#include "Func.h"
#include "VMem.h"
#include "Net.h"
#include "BinarySerial.h"
#include <type_traits>
#include <optional>

class kernel
{
    Memtable memtable;
    VMem vmem;
    std::shared_ptr<Mem> mem = std::make_shared<Mem>();
    std::shared_ptr<threadpool> pool = std::make_shared<threadpool>(1);
    std::priority_queue<std::pair<std::shared_future<int>, Attr>, std::vector<std::pair<std::shared_future<int>, Attr>>, OutComp> ress;

public:
    kernel()
    {
        /*
        mem = std::make_shared<Mem>();
        pool = std::make_shared<threadpool>(1);
        */
        std::cout << "kernel 1.0.0\n";
        std::cout << "cxxos 1.0.0\n";
        std::cout << "kernel loaded\n";
        std::cout << "Welcome to the file system simulator.\n";
        std::cout << "Type 'help' for more information.\n";
        std::cout << "Load from file? (y/n)\n";
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "y")
        {
            bash("deSerial");
        }
        else
        {
            std::cout << "Load failed\n";
            bash("null");
        }
        // std::cout<<"kernel loaded\n";
    }

    void run()
    {
        std::string cmd;
        std::cin.ignore();
        /*
        while (true)
        {
            bool flag = true;
            if(getline(std::cin,cmd))
            {
                flag = bash(cmd);
            }
            else
            {
                flag = bash("null");
            }
            if(!flag) break;
        }*/
        while(std::getline(std::cin,cmd))
        {
            bool flag = true;
            if(cmd.empty()) bash("null");
            else flag = bash(cmd);
            if(!flag) break;
        }
    }

    bool bash(std::istringstream &iss)
    {
        std::string args;
        std::vector<std::string> cmds;
        while (iss >> args)
        {
            cmds.push_back(args);
        }
        return bash(cmds);
    }
    bool bash(const std::string &cmd)
    {
        std::istringstream iss(cmd);
        return bash(iss);
    }
    bool bash(const std::vector<std::string> &cmds);
};

bool kernel::bash(const std::vector<std::string> &cmds)
{
    int idx = 0;
    int n = cmds.size();

    /*
    auto getstring=[&](std::string& cmd,bool need=true)->bool
    {
        if(idx>=n)
        {
            if(need) throw std::runtime_error("too few arguments");
            else return false;
        }
        cmd=cmds[idx++];
        return true;
    };

    auto getint=[&](int& cmd,bool need=true)->bool
    {
        if(idx>=n)
        {
            if(need) throw std::runtime_error("too few arguments");
            else return false;
        }
        cmd=std::stoi(cmds[idx++]);
        return true;
    };

    auto getuint=[&](unsigned int& cmd,bool need=true)->bool
    {
        if(idx>=n)
        {
            if(need) throw std::runtime_error("too few arguments");
            else return false;
        }
        cmd=std::stoul(cmds[idx++]);
        return true;
    };
    */

    constexpr auto get = [](auto &&cmd, int &idx, const std::vector<std::string> &strs, bool need = true) -> bool
    {
        if (idx >= strs.size())
        {
            if (need)
                throw std::runtime_error("Missing parameter");
            else
                return false;
        }
        using T = std::decay_t<decltype(cmd)>;
        if constexpr (std::is_same_v<T, std::string>)
        {
            cmd = strs[idx++];
        }
        else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, unsigned int>)
        {
            cmd = std::stoi(strs[idx++]);
        }
        return true;
    };

    try
    {
        std::string cmd;
        cmd = cmds[idx++];
        if (cmd == "&&")
        {
            std::cout << '\n';
            return true;
        }
        // file
        if (cmd == "findFile")
        {
            std::string path;
            std::string name;
            // getstring(path);
            // getstring(name);
            get(path, idx, cmds);
            get(name, idx, cmds);
            auto tmp = mem->findFile(path, name);
            if (tmp == nullptr)
                std::cout << "Not Found\n";
            else
                std::cout << *tmp << '\n';
        }
        else if (cmd == "findDir")
        {
            std::string name;
            // getstring(name);
            get(name, idx, cmds);
            auto tmp = mem->findDir(name);
            if (tmp == nullptr)
                std::cout << "Not Found\n";
            else
                std::cout << *tmp << '\n';
        }
        else if (cmd == "SearchFile")
        {
            std::string name;
            // getstring(name);
            get(name, idx, cmds);
            auto tmps = mem->SearchFile(name);
            if (tmps.empty())
                std::cout << "Not Found\n";
            else
            {
                for (auto &tmp : tmps)
                {
                    std::cout << *tmp << '\n';
                }
            }
        }
        else if (cmd == "SearchDir")
        {
            std::string name;
            // getstring(name);
            get(name, idx, cmds);
            auto tmps = mem->SearchDir(name);
            if (tmps.empty())
                std::cout << "Not Found\n";
            else
            {
                for (auto &tmp : tmps)
                {
                    std::cout << *tmp << '\n';
                }
            }
        }
        else if (cmd == "addFile")
        {
            std::string path;
            std::string name;
            int len;
            std::string data;
            /*
                    getstring(path);
                    getstring(name);
                    getint(len);
                    */
            get(path, idx, cmds);
            get(name, idx, cmds);
            get(len, idx, cmds);
            if (get(data, idx, cmds, false))
                mem->addFile(path, name, len, data);
            else
                mem->addFile(path, name, len);
        }
        else if (cmd == "addDir")
        {
            std::string path;
            std::string name;
            // getstring(path);
            // getstring(name);
            get(path, idx, cmds);
            get(name, idx, cmds);
            mem->addDir(path, name);
        }
        else if( cmd== "chmod")
        {
            std::vector<std::string> args;
            while(idx<n)
            {
                args.push_back(cmds[idx++]);
            }
            std::string name=args.back();
            args.pop_back();
            std::string path=args.back();
            if(path[0]=='/')
            {
                args.pop_back();
                mem->chmod(path,name,args);
            }
            else
            {
                mem->chmod(name,args);
            }
        }
        else if(cmd=="showmod")
        {
            std::string arg1,arg2;
            get(arg1,idx,cmds);
            if(get(arg2,idx,cmds,false))
            {
                mem->showmod(arg1,arg2);
            }
            else
            {
                mem->showmod(arg1);
            }
        }
        else if (cmd == "md")
        {
            std::string name;
            // getstring(name);
            get(name, idx, cmds);
            mem->md(name);
        }
        else if (cmd == "rd")
        {
            std::string name;
            // getstring(name);
            get(name, idx, cmds);
            mem->rd(name);
        }
        else if (cmd == "type")
        {
            std::string name;
            // getstring(name);
            get(name, idx, cmds);
            int len;
            // getint(len);
            if(get(len, idx, cmds,false))
            {
                std::string data;
                if (get(data, idx, cmds, false))
                    mem->type(name, len, data);
                else
                    mem->type(name, len);
                //return true;
            }
            else
            {
                mem->type(name);
                //return true;
            }
            // mem->type(name, len, data);
        }
        else if (cmd == "del")
        {
            std::string name;
            // getstring(name);
            get(name, idx, cmds);
            mem->del(name);
        }
        else if (cmd == "cd")
        {
            std::string name;
            // getstring(name);
            get(name, idx, cmds);
            mem->cd(name);
        }
        else if (cmd == "copy")
        {
            std::string src, dst;
            // getstring(src);
            // getstring(dst);
            get(src, idx, cmds);
            get(dst, idx, cmds);
            mem->copy(src, dst);
        }
        // Mem
        else if (cmd == "compact")
        {
            mem->compact();
        }
        else if (cmd == "status")
        {
            mem->status();
        }
        // Task
        else if (cmd == "submit")
        {

            unsigned int priority;
            unsigned int burst;
            std::string name;
            int arg0, arg1;
            /*
            getuint(priority);
            getuint(burst);
            getstring(name);
            getint(arg0);
            getint(arg1);
            */
            get(priority, idx, cmds);
            get(burst, idx, cmds);
            get(name, idx, cmds);
            get(arg0, idx, cmds);
            get(arg1, idx, cmds);
            std::shared_ptr<Attr> attr = std::make_shared<Attr>(name, priority, burst);
            auto res = pool->submit(attr, opmap[name], arg0, arg1);
            ress.push(std::make_pair(res, *attr));
        }
        else if (cmd == "run")
        {
            pool->run();
            // std::pair<std::future<int>,Attr> res;
            while (!ress.empty())
            {
                auto res = ress.top();
                ress.pop();
                //printf("Task %d %s %d\n", res.second.pid, res.second.name.c_str(), res.first.get());
                //printf("Running task =name : [%s] ; priority: [%d] ; burst [%d] for %d units. Pid = [%d].",task.second.name.c_str(), task.second.priority, task.second.burst,task.second.burst,task.second.pid);
                printf("Running task =name : [%s] ; Pid = [%d] ; Res=[%d]\n", res.second.name.c_str(),res.second.pid, res.first.get());
            }
        }
        else if (cmd == "dir")
        {
            mem->dir();
        }
        else if (cmd == "exit")
        {
            std::cout << "Save to file? (y/n)\n";
            std::string cmd;
            std::cin >> cmd;
            if (cmd == "y") bash("Serial");
            return false;
        }

        else if (cmd == "debug")
        {
            // mem.debug();
            std::cout << *mem;
        }
        else if (cmd == "Serial")
        {
            // Mem::toSerial("D:\\MY_CODE\\CPP\\OS\\file\\Mem.txt",mem);
            // Mem::toSerial(memtable.generate(),*mem);
            std::stringstream ss;
            ss << *mem;
            std::string str = ss.str();
            BinarySerial::BinarytoSerial(memtable.generate(), str);
        }
        else if (cmd == "deSerial")
        {
            memtable.show();
            std::cout << "input the file idx: ";
            int idx;
            std::cin >> idx;
            auto res = memtable[idx];
            if (res.has_value())
            {
                std::string str = BinarySerial::ChooseSerial(res.value().filename);
                std::stringstream ss(str);
                ss >> *mem;
            }
            else
                bash(cmd);
        }
        else if (cmd == "VMemget")
        {
            /*
            int address;
            std::cout<<"input address:eg(0xffff): ";
            iss>>std::hex>>address;
            vmem.get(address);
            */
            std::cout << "from file get? (y/n)\n";
            std::string cur;
            std::cin >> cur;
            if (cur == "y")
            {
                // std::cout<<"input file name: ";
                std::string filename = "../file/address.txt";
                // iss>>filename;
                std::ifstream fin(filename);
                if (!fin.is_open())
                {
                    std::cout << "file not found\n";
                }
                else
                {
                    std::string line;
                    while (std::getline(fin, line))
                    {
                        std::stringstream ss(line);
                        int address;
                        ss >> std::hex >> address;
                        vmem.get(address);
                    }
                }
            }
            else
            {
                int address;
                while (true)
                {
                    std::cout << "input address:eg(0xffff): ";
                    std::cin >> std::hex >> address;
                    if (address == -1)
                        break;
                    vmem.get(address);
                }
            }
        }
        else if (cmd == "VMemstatus")
        {
            vmem.status();
        }
        else if (cmd == "VMemreset")
        {
            vmem.reset();
        }
        else if (cmd == "Memtableerase")
        {
            int curidx = 0;
            // getint(idx);
            get(curidx, idx, cmds);
            memtable.erase(curidx);
        }
        else if (cmd == "Memtableshow")
        {
            memtable.show();
        }
        else if (cmd == "net")
        {
            try
            {
                Net net(mem);
                net.run();
            }
            catch (const std::exception &e)
            {
                // std::cerr << e.what() << '\n';
            }
        }
        else if (cmd == "help")
        {
            std::cout << "findFile <path> <name> : find file\n";
            std::cout << "findDir <name> : find directory\n";
            std::cout << "SearchFile <name> : search file\n";
            std::cout << "SearchDir <name> : search directory\n";
            std::cout << "addFile <path> <name> <len> <data> : add file\n";
            std::cout << "addDir <path> <name> : add directory\n";
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
            std::cout << "Serial : Serial\n";
            std::cout << "deSerial : deSerial\n";
            std::cout << "VMemget <oxffff> : VMemget\n";
            std::cout << "VMemstatus : VMemstatus\n";
            std::cout << "VMemreset : VMemreset\n";
            std::cout << "Memtablerase <idx> : Memtablerase\n";
            std::cout << "Memtableshow : Memtableshow\n";
            std::cout << "net : net\n";
            std::cout << "help : help\n";
        }
        else if (cmd == "null")
        {

        }
        else
        {
            std::cerr << "Error: Invalid bash.\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    std::string temp;
    temp = std::string("cxx@CXX:") + mem->getcurdir() + std::string("$: ");
    std::cout << temp;
    //std::cout<<"cmd"<<cmds[0]<<"cmd: ";
    return true;
}

#include "Mem.h"
#include "threadpool.h"
#include "Func.h"
#include "VMem.h"
#include "Net.h"
#include "BinarySerial.h"
#include "mount.h"
#include "Msi.h"
#include "CPU.h"
#include <type_traits>
#include <optional>
#include <unordered_set>

#define CXXOS_ARCH "x86_64"
#define CXXOS_VERSION "5.2.0"

static std::unordered_map<std::string,std::string> kernelinfos
{
    {"kernel",CXXOS_VERSION},
    {"cxxos",CXXOS_VERSION},
    {"arch",CXXOS_ARCH},
    {"date",__DATE__},
    {"time",__TIME__},
    {"file",__FILE__}
};

static std::unordered_map<std::string,std::string> userinfos 
{
    {"author","cxx"},
    {"email","cuixiangxiang@sjtu.edu.cn"},
    {"github","https://github.com/cxx221016"}
};

class kernel
{
    Memtable memtable;
    #ifdef CXX_VMEM
    VMem vmem;
    #endif
    std::shared_ptr<Mem> mem = std::make_shared<Mem>();
    #ifdef CXX_THREADPOOL
    std::shared_ptr<threadpool> pool = std::make_shared<threadpool>(1);
    std::priority_queue<std::pair<std::shared_future<int>, Attr>, std::vector<std::pair<std::shared_future<int>, Attr>>, OutComp> ress;
    #endif 
    #ifdef CXX_MOUNT
    std::shared_ptr<Mount> mount = std::make_shared<Mount>();
    #endif
    #ifdef CXX_MSI
    std::shared_ptr<Msi> msi = std::make_shared<Msi>();
    #endif
    #ifdef CXX_CPU
    std::shared_ptr<Process> process = std::make_shared<Process>(1, 0.5);
    #endif
    std::vector<std::string> log;
    static void infos()
    {
        std::cout<<"kernel "<<kernelinfos["kernel"]<<'\n';
        std::cout<<"cxxos "<<kernelinfos["cxxos"]<<'\n';
        std::cout<<"author "<<userinfos["author"]<<'\n';
        std::cout<<"email "<<userinfos["email"]<<'\n';
        std::cout<<"github "<<userinfos["github"]<<'\n';
        std::cout<<"kernel build time "<<kernelinfos["date"]<<' '<<kernelinfos["time"]<<'\n';
        std::cout<<"kernel file "<<kernelinfos["file"]<<'\n';
        std::cout << "kernel loaded\n";
        std::cout << "Welcome to the file system simulator.\n";
        std::cout << "Type 'help' for more information.\n";
    }

public:
    kernel()
    {
        infos();
        std::cout << "Load from file? (y/n)\n";
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "y")
        {
            bash("deSerial");
            log.push_back("load from file");
            std::cout<<"please input your password of user "<<mem->usrname<<": ";
            std::string password;
            std::cin>>password;
            if(mem->login(mem->usrname,password))
            {
                std::cout<<"login success\n";
                bash("null");
            }
            else
            {
                mem->usrname="null";
                std::cout<<"login failed\n";
            }

        }
        else
        {
            //bash("null");
            log.push_back("no load");
            std::cout<<"please input your password of user "<<mem->usrname<<": ";
            std::string password;
            std::cin>>password;
            if(mem->login(mem->usrname,password))
            {
                std::cout<<"login success\n";
                bash("null");
            }
            else
            {
                mem->usrname="null";
                std::cout<<"login failed\n";
            }

        }
        // std::cout<<"kernel loaded\n";
    }

    ~kernel()
    {
        #ifdef CXX_LOG
        static std::string base = "..\\file\\Log";
        static std::string ext = ".txt";
        std::time_t t = std::time(nullptr);
        auto tm_t = std::localtime(&t);
        std::string time = std::to_string(tm_t->tm_year + 1900) + std::to_string(tm_t->tm_mon + 1) + std::to_string(tm_t->tm_mday) + "-" + std::to_string(tm_t->tm_hour) + std::to_string(tm_t->tm_min) + std::to_string(tm_t->tm_sec);
        // std::string time="1:1";
        std::string thisfilename = base + time + ext;
        std::ofstream ofs(thisfilename);
        for (auto &i : log)
        {
            ofs << i << '\n';
        }
        ofs.close();
        #endif
    }

    void run()
    {
        std::string cmd;
        std::cin.ignore();

        while (std::getline(std::cin, cmd))
        {
            bool flag = true;
            if (cmd.empty())
            {
                bash("null");
                log.push_back("null");
            }
            else
            {
                flag = bash(cmd);
                log.push_back(cmd);
            }
            if (!flag)
                break;
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
    if(mem->usrname=="null"&& cmds[0]!="login" && cmds[0]!="exit")
    {
        std::cout<<"please login\n";
        return true;
    }
    int idx = 0;
    int n = cmds.size();

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
        else if constexpr (std::is_same_v<T, double>)
        {
            cmd = std::stod(strs[idx++]);
        }
        else if constexpr (std::is_same_v<T, bool>)
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
        if(cmd == "logout")
        {
            mem->logout();
            std::cout<<"logout success\n";
            return true;
        }
        else if(cmd == "login")
        {
            int n=cmds.size();
            if(n!=3)
            {
                std::cout<<"login failed\n";
                return true;
            }
            std::string usr;
            std::string password;
            get(usr, idx, cmds);
            get(password, idx, cmds);
            if(mem->login(usr,password)) std::cout<<"login success\n";
            else
            {
                std::cout<<"login failed\n";
                bash("logout");
                return true;
            }
        }
        else if(cmd =="addusr")
        {
            if(mem->usrname!="root")
            {
                std::cout<<"you are not root\n";
                bash("null");
                return true;
            }
            std::string usr;
            std::string password;
            get(usr, idx, cmds);
            get(password, idx, cmds);
            mem->addusr(usr,password);
        }
        else if(cmd =="delusr")
        {
            if(mem->usrname!="root")
            {
                std::cout<<"you are not root\n";
                bash("null");
                return true;
            }
            std::string usr;
            get(usr, idx, cmds);
            mem->delusr(usr);
        }
        else if(cmd =="changeusr")
        {
            std::string usr;
            std::string password;
            get(usr, idx, cmds);
            get(password, idx, cmds);
            mem->login(usr,password);
        }
        else if (cmd == "findFile")
        {
            std::string init;
            get(init, idx, cmds);
            auto tmp = mem->findFile(init);
            if (tmp == nullptr)
                std::cout << "Not Found\n";
            else
                std::cout << *tmp << '\n';
        }
        else if (cmd == "findDir")
        {
            std::string init;
            get(init, idx, cmds);
            auto tmp = mem->findDir(init);
            if (tmp == nullptr)
                std::cout << "Not Found\n";
            else
                std::cout << *tmp << '\n';
        }
        else if (cmd == "SearchFile")
        {
            std::string name;
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
        else if (cmd == "chmod")
        {
            std::vector<std::string> args;
            std::string init;
            while (get(init, idx, cmds, false))
            {
                args.push_back(init);
            }
            init = args.back();
            args.pop_back();
            mem->chmod(init, args);
        }
        else if (cmd == "showmod")
        {
            std::string init;
            get(init, idx, cmds);
            mem->showmod(init);
        }
        else if(cmd=="chmon")
        {
            std::string init;
            std::string usr;
            std::string tmpflag;
            get(init, idx, cmds);
            get(usr, idx, cmds);
            if(get(tmpflag, idx, cmds,false)&&tmpflag=="-")
            {
                 mem->chmon(init,usr,false);
            }
            else mem->chmon(init,usr);
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
            if (get(len, idx, cmds, false))
            {
                std::string data;
                if (get(data, idx, cmds, false))
                    mem->type(name, len, data);
                else
                    mem->type(name, len);
                // return true;
            }
            else
            {
                mem->type(name);
                // return true;
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
        else if (cmd == "ren")
        {
            std::string inst;
            std::string name;
            get(inst, idx, cmds);
            get(name, idx, cmds);
            mem->ren(inst, name);
        }
        else if (cmd == "edit")
        {
            std::string init;
            get(init, idx, cmds);
            mem->edit(init);
            return true;
        }
        else if (cmd == "fc")
        {
            std::string args1, args2;
            get(args1, idx, cmds);
            get(args2, idx, cmds);
            mem->fc(args1, args2);
        }
        else if (cmd == "cat")
        {
            std::string init;
            get(init, idx, cmds);
            mem->cat(init);
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
        else if (cmd == "cd")
        {
            std::string name;
            // getstring(name);
            get(name, idx, cmds);
            mem->cd(name);
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
        #ifdef CXX_THREADPOOL
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
                // printf("Task %d %s %d\n", res.second.pid, res.second.name.c_str(), res.first.get());
                // printf("Running task =name : [%s] ; priority: [%d] ; burst [%d] for %d units. Pid = [%d].",task.second.name.c_str(), task.second.priority, task.second.burst,task.second.burst,task.second.pid);
                printf("Running task =name : [%s] ; Pid = [%d] ; Res=[%d]\n", res.second.name.c_str(), res.second.pid, res.first.get());
            }
        }
        #endif
        #ifdef CXX_CPU
        else if(cmd=="top")
        {
            process->top();
        }
        else if(cmd=="kill")
        {
            std::string pid;
            get(pid, idx, cmds);
            if(Process::isnum(pid)) process->kill(std::stoi(pid));
            else process->kill(pid);
        }
        else if(cmd=="create")
        {
            std::string name;
            std::string args;
            bool cur=true;
            get(name, idx, cmds);
            get(args, idx, cmds);
            get(cur, idx, cmds, false);
            if(Process::isnum(args)) process->create(name,std::stoi(args),cur);
            else process->create(name,args,cur);
        }
        else if(cmd=="add")
        {
            std::string args;
            int CPU,Memory,Disk,Network,GPU;
            get(args, idx, cmds);
            get(CPU, idx, cmds,false);
            get(Memory, idx, cmds,false);
            get(Disk, idx, cmds,false);
            get(Network, idx, cmds,false);
            get(GPU, idx, cmds,false);
            if(Process::isnum(args)) process->add(std::stoi(args),CPU,Memory,Disk,Network,GPU);
            else process->add(args,CPU,Memory,Disk,Network,GPU);
        }
        else if(cmd=="remove")
        {
            std::string args;
            get(args, idx, cmds);
            int newstatus;
            if(get(newstatus, idx, cmds,false))
            {
                if(Process::isnum(args)) process->remove(std::stoi(args),newstatus);
                else process->remove(args,newstatus);
            }
            else
            {
                if(Process::isnum(args)) process->remove(std::stoi(args));
                else process->remove(args);
            }
        }
        else if(cmd=="runstatus")
        {
            process->runningstatus();
        }
        #endif
        else if (cmd == "dir")
        {
            mem->dir();
        }
        else if(cmd=="user")
        {
            std::cout<<"user:"<<mem->usrname<<std::endl;
        }
        else if (cmd == "exit")
        {
            std::cout << "Save to file? (y/n)\n";
            std::string cmd;
            std::cin >> cmd;
            if (cmd == "y")
                bash("Serial");
            return false;
        }
        else if (cmd == "debug")
        {
            // mem.debug();
            std::cout << *mem;
        }
        else if(cmd == "version")
        {
            std::cout<<"version:" << CXXOS_VERSION <<" "<<"arch:"<<CXXOS_ARCH<<std::endl;
        }
        else if(cmd=="capacity")
        {
            std::cout<<"capacity:"<<mem->getcapacity()<<std::endl;
        }
        else if(cmd=="time")
        {
            std::time_t t = std::time(nullptr);
            auto tm_t = std::localtime(&t);
            std::stringstream ss;
            ss << std::put_time(tm_t, "%Y-%m-%d %H:%M:%S");
            std::string str = ss.str();
            std::cout<<str<<std::endl;
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

            static bool flag = false;
            if(!flag)
            {
                flag = true;
                return true;
            }
        }
        #ifdef CXX_VMEM
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
                    return true;
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
        #endif
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
        #ifdef _WIN32
        #ifdef CXX_NET
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
        #endif
        #ifdef CXX_MOUNT
        else if(cmd=="mountadd")
        {
            if(cmds.size()==1) mount->add();
            else 
            {
                std::string tmp;
                for(int i=1;i<cmds.size();i++)
                {
                    tmp+=cmds[i];
                    tmp+=" ";
                }
                mount->add(tmp);
            }
        }
        else if(cmd=="mountlist")
        {
            mount->list();
        }
        else if(cmd=="mountremove")
        {
            std::string tmp;
            get(tmp, idx, cmds);
           try 
            {
                int id=std::stoi(tmp);
                mount->remove(id);
            }
            catch(const std::exception& e)
            {
                mount->remove(tmp);
            }
        }
        else if(cmd=="mountclear")
        {
            mount->clear();
        }
        else if(cmd=="mountopen")
        {
            std::string tmp;
            get(tmp, idx, cmds);
            try 
            {
                int id=std::stoi(tmp);
                mount->open(id);
            }
            catch(const std::exception& e)
            {
                mount->open(tmp);
            }
        }
        #endif
        #endif
        #ifdef CXX_MSI
        else if(cmd== "pip")
        {
            std::string tmp;
            get(tmp, idx, cmds);
            if(tmp=="list")
            {
                msi->piplist();
            }
            else if(tmp=="show")
            {
                std::string name;
                get(name, idx, cmds);
                msi->pipshow(name);
            }
            else if(tmp=="install")
            {
                std::string name;
                get(name, idx, cmds);
                msi->pipinstall(name);
            }
            else if(tmp=="uninstall")
            {
                std::string name;
                get(name, idx, cmds);
                msi->pipuninstall(name);
            }
            else if(tmp=="upgrade")
            {
                std::string name;
                get(name, idx, cmds);
                msi->pipupgrade(name);
            }
        }
        #endif
        else if (cmd == "help")
        {
            std::unordered_set<std::string> cmdset{cmds.begin() + 1, cmds.end()};
            if(cmdset.empty())
            {
                std::cout << "help\n";
                std::cout << "help -system: show system commands\n";
                std::cout << "help -global: show global commands\n";
                std::cout << "help -fileop: show fileop commands\n";
                std::cout << "help -memop: show memop commands\n";
                #ifdef CXX_MOUNT
                std::cout <<"help -mountop: show mountop commands\n";
                #endif
                #ifdef CXX_THREADPOOL
                std::cout<<"help -threadpoolop: show threadpoolop commands\n";
                #endif
                #ifdef CXX_MSI
                std::cout<<"help -msiop: show msiop commands\n";
                #endif
                #ifdef CXX_VMEM
                std::cout << "help -vmemop: show vmemop commands\n";
                #endif
                std::cout << "help -memtableop: show memtableop commands\n";
                #ifdef CXX_NET
                std::cout << "help -netop: show netop commands\n";
                #endif
            }
            //system
            if (cmdset.count("-system"))
            {
                std::cout << "system\n";
                std::cout << "user: show user\n";
                std::cout << "login <usrname> <password>: login\n";
                std::cout << "logout: logout\n";
                std::cout << "changeusr <usrname> <password>: change user\n";
                std::cout << "exit: exit\n";
                std::cout << "debug: show mem\n";
                std::cout << "version: show version\n";
                std::cout << "capacity: show capacity\n";
                std::cout << "time : get time\n";
            }

            // global
            if (cmdset.count("-global"))
            {
                std::cout << "global\n";
                std::cout << "findFile <filename>: find file\n";
                std::cout << "findDir <dirname>: find dir\n";
                std::cout << "SearchFile <filename>: search file\n";
                std::cout << "SearchDir <dirname>: search dir\n";
                std::cout << "chmod  <mode> <filename>: change file mode\n";
                std::cout << "showmod <filename>: show file mode\n";
                std::cout << "ren <filename> <newname>: rename file\n";
            }
            // fileop
            if (cmdset.count("-fileop"))
            {
                std::cout << "fileop\n";
                std::cout << "type <filename> <len(optional)> <data(optional)>: show file content\n";
                std::cout << "del <filename>: delete file\n";
                std::cout << "edit <filename>: edit file content\n";
                std::cout << "fc <filename> <filename>: compare two files\n";
                std::cout << "cat <filename> <filename>: cat two files\n";
                std::cout << "copy <filename> <filename>: copy file\n";
                std::cout << "move <filename> <filename>: move file\n";
            }
            // dirop
            if (cmdset.count("-dirop"))
            {
                std::cout << "dirop\n";
                std::cout << "cd <dirname>: change dir\n";
                std::cout << "dir: show dir\n";
                std::cout << "md <filename>: make file\n";
                std::cout << "rd <filename>: remove file\n";
            }

            // Memtable load
            if (cmdset.count("-Memtableop"))
            {
                std::cout << "Memtableop\n";
                std::cout << "Serial: save to file\n";
                std::cout << "deSerial: load from file\n";
                std::cout << "Memtableerase <idx>: erase memtable\n";
                std::cout << "Memtableshow: show memtable\n";
            }

            // thread
            #ifdef CXX_THREADPOOL
            if (cmdset.count("-threadop"))
            {
                std::cout << "threadop\n";
                std::cout << "submit <priority> <burst> <name> <arg1> <arg2> : submit a thread\n";
                std::cout << "run : run a thread\n";
            }
            #endif

            // VMem
            #ifdef CXX_VMEM
            if (cmdset.count("-VMemop"))
            {
                std::cout << "VMemop\n";
                std::cout << "VMemget: get vmem\n";
                std::cout << "VMemstatus: show vmem status\n";
                std::cout << "VMemreset: reset vmem\n";
            }
            #endif

            // net
            #ifdef CXX_NET
            #ifdef _WIN32
            if (cmdset.count("-netop"))
            {
                std::cout << "netop\n";
                std::cout << "net: run net\n";
            }
            #endif
            #endif

            #ifdef CXX_MSI
            if(cmdset.count("-msiop"))
            {
                std::cout<<"msiop\n";
                std::cout<<"pip install <filename>: install package\n";
                std::cout<<"pip uninstall <filename>: uninstall package\n";
                std::cout<<"pip list: list packages\n";
                std::cout<<"pip show <filename>: search package\n";
                std::cout<<"pip upgrade <filename>: upgrade package\n";
            }
            #endif
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
    // std::cout<<"cmd"<<cmds[0]<<"cmd: ";
    return true;
}

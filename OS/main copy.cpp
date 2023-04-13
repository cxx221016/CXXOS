#include "include/Mem.h"
#include "include/threadpool.h"
#include "include/Func.h"
#include "include/VMem.h"
#include "include/Net.h"
#include "include/BinarySerial.h"

/*
cmake -G "MinGW Makefiles" ..
*/
Memtable memtable;
VMem vmem;
//Mem mem;
std::shared_ptr mem=std::make_shared<Mem>();
//Net net(std::make_shared<Mem>(mem));
threadpool pool(1);
std::priority_queue<std::pair<std::shared_future<int>, Attr>, std::vector<std::pair<std::shared_future<int>, Attr>>, OutComp> ress;
bool command(std::string &cmd);

int main()
{
    std::string cmd;
    std::cout << "kernel 1.0.0\n";
    std::cout << "cxxos 1.0.0\n";
    std::cout << "kernel loaded\n";
    std::cout << "Welcome to the file system simulator.\n";
    std::cout << "Type 'help' for more information.\n";
    std::cout << "Load from file? (y/n)\n";
    std::cin>>cmd;
    if(cmd=="y")
    {
        //Mem::deSerial("D:\\MY_CODE\\CPP\\OS\\file\\Mem.txt",mem);
        /*
        std::cout<<"input file timestamp: ";
        std::string time;
        std::cin>>time;
        Mem::deSerial(generate(time),mem);
        std::cout<<"Load success\n";
        */
        std::string cmd="deSerial";
        command(cmd);
    }
    else
    {
        std::cout<<"Load failed\n";
    }
    std::string temp;
    temp = std::string("cxx@CXX:") + mem->getcurdir() + std::string("$: ");
    std::cout << temp;
    while(std::cin>>cmd)
    {
        bool flag=command(cmd);
        if(!flag) break;
    }
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
    if(cmd=="findFile")
    {
        std::string path;
        std::string name;
        std::cin>>path>>name;
        auto tmp=mem->findFile(path,name);
        if(tmp==nullptr) std::cout<<"Not Found\n";
        else std::cout<<*tmp<<'\n';
    }
    else if(cmd=="findDir")
    {
        std::string name;
        std::cin>>name;
        auto tmp=mem->findDir(name);
        if(tmp==nullptr) std::cout<<"Not Found\n";
        else std::cout<<*tmp<<'\n';
    }
    else if(cmd=="SearchFile")
    {
        std::string name;
        std::cin>>name;
        auto tmps=mem->SearchFile(name);
        if(tmps.empty()) std::cout<<"Not Found\n";
        else
        {
            for(auto &tmp:tmps)
            {
                std::cout<<*tmp<<'\n';
            }
        }
    }
    else if(cmd=="SearchDir")
    {
        std::string name;
        std::cin>>name;
        auto tmps=mem->SearchDir(name);
        if(tmps.empty()) std::cout<<"Not Found\n";
        else
        {
            for(auto &tmp:tmps)
            {
                std::cout<<*tmp<<'\n';
            }
        }
    }
    else if(cmd=="addFile")
    {
        std::string path;
        std::string name;
        int len;
        std::string data;

        std::cin>>path>>name>>len>>data;
        mem->addFile(path,name,len,data);
    }
    else if(cmd=="addDir")
    {
        std::string path;
        std::string name;
        std::cin>>path>>name;
        mem->addDir(path,name);
    }
    else if (cmd == "md")
    {
        std::string name;
        std::cin >> name;
        mem->md(name);
    }
    else if (cmd == "rd")
    {
        std::string name;
        std::cin >> name;
        mem->rd(name);
    }
    else if (cmd == "type")
    {
        std::string name;
        std::cin >> name;
        int len;
        std::cin >> len;
        std::string data;
        std::cin >> data;
        mem->type(name, len, data);
    }
    else if (cmd == "del")
    {
        std::string name;
        std::cin >> name;
        mem->del(name);
    }
    else if (cmd == "cd")
    {
        std::string name;
        std::cin >> name;
        mem->cd(name);
    }
    else if(cmd=="copy")
    {
        std::string src,dst;
        std::cin>>src>>dst;
        mem->copy(src,dst);
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
        mem->dir();
    }
    else if (cmd == "exit")
    {
        std::cout<<"Save to file? (y/n)\n";
        std::string cmd;
        std::cin>>cmd;
        if(cmd=="y") Mem::toSerial(memtable.generate(),*mem);
        return false;
    }

    else if (cmd == "debug")
    {
        //mem.debug();
        std::cout<<*mem;
    }
    else if(cmd=="Serial")
    {
        //Mem::toSerial("D:\\MY_CODE\\CPP\\OS\\file\\Mem.txt",mem);
        //Mem::toSerial(memtable.generate(),*mem);
        std::stringstream ss;
        ss<<*mem;
        std::string str=ss.str();
        BinarySerial::BinarytoSerial(memtable.generate(),str);

    }
    else if(cmd=="deSerial")
    {
        memtable.show();
        std::cout<<"input the file idx: ";
        int idx;
        std::cin>>idx;
        auto res=memtable[idx];
        if(res.has_value()) 
        {
            std::string str=BinarySerial::ChooseSerial(res.value().filename);
            std::stringstream ss(str);
            ss>>*mem;
        }
        else command(cmd);
    }
    else if(cmd == "VMemget")
    {
        /*
        int address;
        std::cout<<"input address:eg(0xffff): ";
        std::cin>>std::hex>>address;
        vmem.get(address);
        */
        std::cout<<"from file get? (y/n)\n";
        std::string cur;
        std::cin>>cur;
        if(cur=="y")
        {
            //std::cout<<"input file name: ";
            std::string filename="../file/address.txt";
            //std::cin>>filename;
            std::ifstream fin(filename);
            if(!fin.is_open())
            {
                std::cout<<"file not found\n";
            }
            else
            {
                std::string line;
                while(std::getline(fin,line))
                {
                    std::stringstream ss(line);
                    int address;
                    ss>>std::hex>>address;
                    vmem.get(address);
                }
            }
        }
        else
        {
            int address;
            while(true)
            {
                std::cout<<"input address:eg(0xffff): ";
                std::cin>>std::hex>>address;
                if(address==-1) break;
                vmem.get(address);
            }
        }

    }
    else if(cmd == "VMemstatus")
    {
        vmem.status();
    }
    else if(cmd =="VMemreset")
    {
        vmem.reset();
    }
    else if(cmd =="Memtablerase")
    {
        int idx=0;
        std::cin>>idx;
        memtable.erase(idx);
    }
    else if(cmd =="Memtableshow")
    {
        memtable.show();
    }
    else if(cmd =="net")
    {
        try
        {
            Net net(mem);
            net.run();
        }
        catch(const std::exception& e)
        {
            //std::cerr << e.what() << '\n';
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
    else
    {
        std::cerr << "Error: Invalid command.\n";
    }

    // std::cout<<"cxx@CXX:/CXX$: ";
    std::string temp;
    temp = std::string("cxx@CXX:") + mem->getcurdir() + std::string("$: ");
    std::cout << temp;
    return true;
}
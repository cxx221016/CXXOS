#pragma once
#define CXX_PROCESS
#ifdef CXX_CPU
#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<unordered_map>
#include<memory>
#include<stack>
#include<sstream>
#include<fstream>
#include<thread>
#include<cstdlib>
#include<type_traits>
#include<unordered_set>
#include<functional>
#include<cstring>
using namespace std::string_literals;
 enum 
{
    NEW,
    READY,
    RUN,
    WAIT,
    EXIT
};

static std::unordered_map<int,std::string> status_table=
{
    {0,"new"},
    {1,"ready"},
    {2,"run"},
    {3,"wait"},
    {4,"exit"}
};

struct ProcessNode
{
    std::string name;
    int pid;
    int status;
    int CPU,Memory,Disk,Network,GPU;
    std::shared_ptr<ProcessNode> parent;
    std::vector<std::shared_ptr<ProcessNode>> children;
    ProcessNode(const std::string& name,int pid,std::shared_ptr<ProcessNode> parent,int  status=NEW,int CPU=0,int Memory=0,int Disk=0,int Network=0,int GPU=0)
        :name(name),pid(pid),parent(parent),status(status),CPU(CPU),Memory(Memory),Disk(Disk),Network(Network),GPU(GPU)
    {}
    ProcessNode()=default;
    ProcessNode(const ProcessNode&)=default;
    ProcessNode(ProcessNode&&)=default;
    ProcessNode& operator=(const ProcessNode&)=default;
    ProcessNode& operator=(ProcessNode&&)=default;
    ~ProcessNode()=default;
    friend std::ostream& operator<<(std::ostream& os,const ProcessNode& node)
    {
        os<<node.name<<' '<<node.pid<<' '<<node.status<<' '<<node.CPU<<' '<<node.Memory<<' '<<node.Disk<<' '<<node.Network<<' '<<node.GPU;
        return os;
    }
    friend std::istream& operator>>(std::istream& is,ProcessNode& node)
    {
        is>>node.name>>node.pid>>node.status>>node.CPU>>node.Memory>>node.Disk>>node.Network>>node.GPU;
        return is;
    }
    friend bool operator<(const std::shared_ptr<ProcessNode>& lhs,const std::shared_ptr<ProcessNode>& rhs)
    {
        if(lhs->CPU==rhs->CPU)
        {
           if(lhs->Memory==rhs->Memory)
           {
               if(lhs->Disk==rhs->Disk)
               {
                   if(lhs->Network==rhs->Network)
                   {
                       return lhs->GPU<rhs->GPU;
                   }
                   return lhs->Network<rhs->Network;
               }
               return lhs->Disk<rhs->Disk;
           }
              return lhs->Memory<rhs->Memory;
         }
        return lhs->CPU<rhs->CPU;
    }
};


class Process
{
private:
    std::unordered_map<std::string,std::shared_ptr<ProcessNode>> name_table;
    std::unordered_map<int,std::shared_ptr<ProcessNode>> pid_table;
    std::priority_queue<std::shared_ptr<ProcessNode>> pri_queue;

    int pidcnt=1;
    double time_slice;
    std::shared_ptr<ProcessNode> running;
    std::unordered_set<std::shared_ptr<ProcessNode>> ready_queue;
    double cpu=0,mem=0,disk=0,network=0,gpu=0;
    double wave;
public:
    template<typename T>
    std::shared_ptr<ProcessNode> findNode(T&& args);

    template<typename T>
    bool kill(T&& args);

    template<typename T>
    std::shared_ptr<ProcessNode> create(const std::string& name,T&& args,bool flag=true);
    Process(double time_slice=10,double wave=5);
    ~Process() = default;
    Process(const Process&)=delete;
    Process(Process&&)=delete;
    Process& operator=(const Process&)=delete;
    Process& operator=(Process&&)=delete;

    void top(int n=10);

    template<typename T>
    void add(T&& args,int CPU=40,int Memory=2,int Disk=2,int Network=2,int GPU=2);

    template<typename T>
    void change(T&& args,int CPU=0,int Memory=0,int Disk=0,int Network=0,int GPU=0);

    template<typename T>
    void remove(T&& args,int newstatus=EXIT);

    int Wave(int args);

    template <typename F, typename... Args>
    void Invoke(double delaytime, F &&f, Args &&...args)
    {
        //std::function<void()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        std::thread tmp([=]()
                    {
                        std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(delaytime)));
                        //func(); 
                        //std::invoke(f, args...);
                    });
        tmp.detach();
    }

    static bool isnum(const std::string& str)
    {
        try
        {
            int num = std::stoi(str);
            return true;
        }
        catch (const std::exception& e)
        {
            return false;
        }
    }

    template<typename T>
    void changerunning(T&& args);

    void runningstatus();
};


#include "../src/CPU.cpp"
#endif
//#include"../include/CPU.h"
//#define CXX_CPU
#ifdef CXX_CPU

Process::Process(double time_slice,double wave)
    :time_slice(time_slice),wave(wave)
{
    running=create("init","null"s);
    running=create("sysinit","init"s);
    running=create("userinit","init"s);
    running=create("user","userinit"s);
    running=create("local","user"s);
    running=create("remote","user"s);
    running=create("test","user"s);
    running=create("vscode","local"s);
    running=create("edge","local"s);
    running=create("wechat","local"s);
    running=create("qq","local"s);
    running=create("cpp-tools","vscode"s);
    running=create("git","vscode"s);
    running=create("python","vscode"s);
    running=create("node","vscode"s);
}

template<typename T>
std::shared_ptr<ProcessNode> Process::findNode( T&& args)
{
    //std::cout<<args<<std::endl;
    using F=std::decay_t<T>;
    //std::cout<<(std::is_same_v<F,std::string>)<<std::endl;
    if constexpr(std::is_same_v<F,std::string>||std::is_same_v<F,const char*>||std::is_same_v<F,const char[]>)
    {
        auto it=name_table.find(args);
        if(it!=name_table.end())
            return it->second;
    }
    else if constexpr(std::is_same_v<F,int>)
    {
        auto it=pid_table.find(args);
        if(it!=pid_table.end())
            return it->second;
    }
    else if constexpr(std::is_same_v<F,std::shared_ptr<ProcessNode>>)
    {
        return args;
    }
    return nullptr;
}

template<typename T>
bool Process::kill(T&& args)
{
    auto node=findNode(args);
    if(node==nullptr)
        return false;
    node->status=EXIT;
    name_table.erase(node->name);
    pid_table.erase(node->pid);
    return true;
}

template<typename T>
std::shared_ptr<ProcessNode> Process::create(const std::string& name,T&& args,bool flag)
{
    //auto parent=findNode(args);
    auto parent=findNode(args);
    if (parent == nullptr)
    {
        if(pidcnt!=1) return nullptr;
    }
    auto node=std::make_shared<ProcessNode>(name,pidcnt++,parent);
    //std::cout<<"create "<<*node<<std::endl;
    if(parent) parent->children.push_back(node);
    name_table[name]=node;
    pid_table[node->pid]=node;
    if(flag)
    {
        pri_queue.push(node);
        ready_queue.insert(node);
    }
    return node;
}

void Process::top(int n)
{
    std::queue<std::shared_ptr<ProcessNode>> tmp;
    while(!pri_queue.empty()&&n--)
    {
        auto node=pri_queue.top();
        pri_queue.pop();
        std::cout<<*node<<std::endl;
        tmp.push(node);
    }
    while(!tmp.empty())
    {
        pri_queue.push(tmp.front());
        tmp.pop();
    }
}

template<typename T>
void Process::add(T&& args,int CPU,int Memory,int Disk,int Network,int GPU)
{
    auto node=findNode(args);
    if(node==nullptr)
        return;
    pri_queue.push(node);
    ready_queue.insert(node);
    Invoke(0,&Process::change<T>,this,args,CPU,Memory,Disk,Network,GPU);
    Invoke(10,&Process::change<T>,this,args,0,0,0,0,0);
}



template<typename T>
void Process::change(T&& args,int CPU,int Memory,int Disk,int Network,int GPU)
{
    auto node=findNode(args);
    if(node==nullptr)
        return;
    node->CPU=Wave(CPU);
    node->Memory=Wave(Memory);
    node->Disk=Wave(Disk);
    node->Network=Wave(Network);
    node->GPU=Wave(GPU);
    if(CPU==0) node->status=EXIT;
}


template<typename T>
void Process::remove(T&& args,int newstatus)
{
    auto node=findNode(args);
    if(node==nullptr)
        return;
    node->status=newstatus;
    std::queue<std::shared_ptr<ProcessNode>> tmp;
    while(!pri_queue.empty())
    {
        auto topnode=pri_queue.top();
        pri_queue.pop();
        if(node==topnode)
            continue;
        tmp.push(topnode);
    }
    while(!tmp.empty())
    {
        pri_queue.push(tmp.front());
        tmp.pop();
    }
    ready_queue.erase(node);
}


int Process::Wave(int args)
{
    if(args==0)
        return 0;
    return args-wave/2+rand()%static_cast<int>(wave);
}

template<typename T>
void Process::changerunning(T&& args)
{
    auto node=findNode(args);
    if(node==nullptr)
        return;
    if(node->status==EXIT)
        return;
    if(running->status==RUN)
    {
        add(node);
    }
    running=node;
    remove(running,RUN);
}

void Process::runningstatus()
{
    std::cout<<*running<<std::endl;
}

#endif
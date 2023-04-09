#include "../include/task.h"

unsigned int  Task::cnt=0;

/*
template<typename F, typename... Args>
Task::Task( unsigned int priority, unsigned int burst,F&& f,Args&&... args)
{
    pid=cnt++;
    impltask = std::bind(std::forward<F>(f),std::forward<Args>(args)...);
    this->priority=priority;
    this->burst=burst;
}*/


Task::~Task()
{

}

void Task::exec()
{
    printf("Running task = [%s] [%d] [%d] for %d units.\n",name.c_str(), priority, burst,burst);
}

unsigned int Task::getPid() const
{
    return pid;
}
unsigned int Task::getPriority() const
{
    return priority;
}
unsigned int Task::getBurst() const
{
    return burst;
}

/*
template<typename T>
struct Comp_burst
{
    Comp_burst(){};
    bool operator()(const T& t1,const T& t2) const
    {
        return t1.getBurst()>t2.getBurst();
    }
};

template<typename T>
struct Comp_priority
{
    Comp_priority(){};
    bool operator()(const T& t1,const T& t2) const
    {
        return t1.getPriority()>t2.getPriority();
    }
};*/


/*
struct Comp_burst
{
    Comp_burst(){}
    bool operator()(const Task& t1,const Task& t2) const
    {
        return t1.getBurst()>t2.getBurst();
    }
};

struct Comp_priority
{
    Comp_priority(){}
    bool operator()(const Task& t1,const Task& t2) const
    {
        return t1.getPriority()>t2.getPriority();
    }
};
*/



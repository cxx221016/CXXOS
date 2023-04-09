#include "../include/Sche.h"

Sche::Sche(const std::string &ops ) : ops(ops)
{
    shutdown = false;
    // schefunc=func;
}
Sche::~Sche()
{
    shutdown = true;
}
void Sche::submit(const Task &task)
{
    tasklist.push_back(task);
}

void Sche::run()
{
    switch (ops[0])
    {
    case 'P':
        Pr_Sche();
        break;
    case 'R':
        RR_Sche();
        break;
    case 'F':
        FCFS_Sche();
        break;
    case 'S':
        SJF_Sche();
        break;
    default:
        break;
    }
}

void Sche::Pr_Sche()
{
    std::priority_queue<Task, std::vector<Task>, Comp_priority<Task>> taskqueue;
    for (const auto &task : tasklist)
    {
        taskqueue.push(task);
    }
    while (!taskqueue.empty())
    {
        auto task = taskqueue.top();
        taskqueue.pop();
        task.exec();
    }
}
void Sche::RR_Sche()
{
    std::queue<Task> taskqueue;
    for (const auto &task : tasklist)
    {
        taskqueue.push(task);
    }
    while (!taskqueue.empty())
    {
        auto task = taskqueue.front();
        taskqueue.pop();
        task.exec();
        taskqueue.push(task);
    }
}
void Sche::FCFS_Sche()
{
    std::priority_queue<Task, std::vector<Task>, Comp_burst<Task>> taskqueue;
    for (const auto &task : tasklist)
    {
        taskqueue.push(task);
    }
    while (!taskqueue.empty())
    {
        auto task = taskqueue.top();
        taskqueue.pop();
        task.exec();
    }
}
void Sche::SJF_Sche()
{
    std::priority_queue<Task, std::vector<Task>, Comp_burst<Task>> taskqueue;
    for (const auto &task : tasklist)
    {
        taskqueue.push(task);
    }
    while (!taskqueue.empty())
    {
        auto task = taskqueue.top();
        taskqueue.pop();
        task.exec();
    }
}
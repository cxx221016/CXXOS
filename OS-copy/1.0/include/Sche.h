#pragma once
#include "Task.h"
#include<vector>
#include<queue>

//using Schefunc=std::function<void()>;

class Sche
{
    std::vector<Task> tasklist;
    bool shutdown;
    std::string ops;
    //Schefunc schefunc;
public:
    Sche(const std::string& ops="Pr");
    ~Sche();
    void submit(const Task& task);
    void run();
    void Pr_Sche();
    void RR_Sche();
    void FCFS_Sche();
    void SJF_Sche();
};


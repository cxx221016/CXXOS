#pragma once
#include <functional>
#include <type_traits>
#include <utility>
#include <iostream>
#include <string>
#include <any>

class Sche;

class Task
{
private:
    std::function<void()> impltask;
    std::string name;
    unsigned int pid;
    unsigned int priority;
    unsigned int burst;
    void exec();
    static unsigned int cnt;

public:
    friend class Sche;

    Task() = default;

    template <typename F, typename... Args>
    Task(std::string name,unsigned int priority, unsigned int burst, F &&f, Args &&...args)
    {
        this->name = name;
        pid = cnt++;
        impltask = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        this->priority = priority;
        this->burst = burst;
    }

    Task(const Task &) = default;
    Task(Task &&) = default;
    Task &operator=(const Task &) = default;
    Task &operator=(Task &&) = default;
    ~Task();
    unsigned int getPid() const;
    unsigned int getPriority() const;
    unsigned int getBurst() const;
};

// cmake -G "MinGW Makefiles" ..

/*
template<typename T>
struct Comp_burst;

template<typename T>
struct Comp_priority;
*/
template <typename T>
struct Comp_burst
{
    Comp_burst(){};
    bool operator()(const T &t1, const T &t2) const
    {
        return t1.getBurst() > t2.getBurst();
    }
};

template <typename T>
struct Comp_priority
{
    Comp_priority(){};
    bool operator()(const T &t1, const T &t2) const
    {
        return t1.getPriority() > t2.getPriority();
    }
};
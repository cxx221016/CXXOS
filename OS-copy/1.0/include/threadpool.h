#pragma
#include <iostream>
#include<thread>
#include<mutex>
#include <chrono>
#include<time.h>
#include<vector>
#include<queue>
#include<future>
#include <mutex>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <utility>
#include <vector>
#include <condition_variable>
#include<string>
#include<shared_mutex>

struct Args
{
    std::string name;
    int priority;
    int burst;
    Args()=default;
    Args(const Args&)=default;
    Args(Args&&)=default;
    Args& operator=(const Args&)=default;
    Args& operator=(Args&&)=default;
    Args(std::string name,int priority,int burst):name(name),priority(priority),burst(burst){}
};

struct Comp
{
    bool operator()(const std::pair<std::function<void()>,Args>& a,const std::pair<std::function<void()>,Args>& b)
    {
        return a.second.priority>b.second.priority;
    }
};

template<typename T,typename Sequence=std::vector<T>,typename Compare=std::less<typename T::value_type>> 
class threadsafe_priority_queue
{
private:
    mutable std::mutex mut;
    std::priority_queue<T,Sequence,Compare> data_queue;
    std::condition_variable data_cond;
public:
    threadsafe_priority_queue(){}
    threadsafe_priority_queue(threadsafe_priority_queue const& other)
    {
        std::lock_guard<std::mutex> lk(other.mut);
        data_queue=other.data_queue;
    }
    void push(T new_value)
    {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_value);
        data_cond.notify_one();
    }
    void pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk,[this]{return !data_queue.empty();});
        value=data_queue.top();
        data_queue.pop();
    }
    std::shared_ptr<T> pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk,[this]{return !data_queue.empty();});
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.top()));
        data_queue.pop();
        return res;
    }
    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if(data_queue.empty()) return false;
        value=data_queue.top();
        data_queue.pop();
        return true;
    }
    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if(data_queue.empty()) return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.top()));
        data_queue.pop();
        return res;
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};

template<typename T>
class threadsafe_queue
{
private:
    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;
public:
    threadsafe_queue(){}
    threadsafe_queue(threadsafe_queue const& other)
    {
        std::lock_guard<std::mutex> lk(other.mut);
        data_queue=other.data_queue;
    }
    void push(T new_value)
    {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_value);
        data_cond.notify_one();
    }
    void pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk,[this]{return !data_queue.empty();});
        value=data_queue.front();
        data_queue.pop();
    }
    std::shared_ptr<T> pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk,[this]{return !data_queue.empty();});
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }
    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if(data_queue.empty()) return false;
        value=data_queue.front();
        data_queue.pop();
        return true;
    }
    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if(data_queue.empty()) return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};

class threadpool
{
private:
    friend class worker;
    class worker
    {
    private:
        threadpool* pool;
    public:
        worker(threadpool* pool=nullptr):pool(pool){}
        void operator()()
        {
            while(!pool->is_shut_down)
            {
                bool flag=false;
                //std::function<void()> task;
                std::pair<std::function<void()>,Args> task;
                std::unique_lock<std::mutex> lk(pool->mtx);
                pool->cv.wait(lk,[this]{return this->pool->is_shut_down||!this->pool->work_queue.empty();});
                flag=pool->work_queue.try_pop(task);
                if(flag) 
                {
                    printf("Running task = [%s] [%d] [%d] for %d units.\n",task.second.name.c_str(), task.second.priority, task.second.burst,task.second.burst);
                    (task.first)();
                }
            }
        }
    };
    bool is_shut_down;
    threadsafe_priority_queue<std::pair<std::function<void()>,Args>,std::vector<std::pair<std::function<void()>,Args>>,Comp> work_queue;
    std::vector<std::thread> threads;
    std::mutex mtx;
    std::condition_variable cv;
public:
    threadpool(int n):threads(n),is_shut_down(true)
    {
        for(auto& th:threads) th=std::thread{worker(this)};
    }
    threadpool(const threadpool&)=delete;
    threadpool& operator=(const threadpool&)=delete;
    threadpool (threadpool&&)=delete;
    threadpool& operator=(threadpool&&)=delete;

    ~threadpool()
    {
        is_shut_down=true;
        cv.notify_all();
        for(auto& th:threads) 
        {
            if(th.joinable()) th.join();
        }
    }
    

    //C++17
    template<typename F,typename... Args,
    typename=std::enable_if_t<std::is_void_v<std::invoke_result_t<std::decay_t<F>,std::decay_t<Args>...>>>>
    std::future<bool> submit(F&& f,Args&&... args)
    {
        auto promise=std::make_shared<std::promise<bool>>();
        auto future=promise->get_future();
        std::function<void()> func=std::bind(std::forward<F>(f),std::forward<Args>(args)...);
        work_queue.push([func,promise]()
        {
            func();
            promise->set_value(true);
        });
        cv.notify_one();
        return future;
    }
    
    
    template<typename F,typename... Args,
    typename R=std::invoke_result_t<std::decay_t<F>,std::decay_t<Args>...>,
    typename=std::enable_if_t<!std::is_void_v<R>>>
    std::future<R> submit(F&& f,Args&&... args)
    {
        auto promise=std::make_shared<std::promise<R>>();
        auto future=promise->get_future();
        std::function<R()> func=std::bind(std::forward<F>(f),std::forward<Args>(args)...);
        work_queue.push([func,promise]()
        {
            promise->set_value(func());
        });
        cv.notify_one();
        return future;
    }

};

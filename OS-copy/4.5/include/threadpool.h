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

struct Attr
{
    std::string name;
    int priority;
    int burst;
    int pid;
    Attr()=default;
    Attr(const Attr&)=default;
    Attr(Attr&&)=default;
    Attr& operator=(const Attr&)=default;
    Attr& operator=(Attr&&)=default;
    Attr(std::string name,int priority,int burst):name(name),priority(priority),burst(burst)
    {
        static int cnt=0;
        pid=cnt++;
    }
};

struct OutComp
{
    bool operator()(const std::pair<std::shared_future<int>,Attr>& a,const std::pair<std::shared_future<int>,Attr>& b)
    {
        return a.second.priority>b.second.priority;
    }
};

struct Comp
{
    bool operator()(const std::pair<std::function<void()>,Attr>& a,const std::pair<std::function<void()>,Attr>& b)
    {
        return a.second.priority>b.second.priority;
    }
};

//static unsigned int cnt=0;

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
        data_queue.emplace(new_value);
        data_cond.notify_one();
    }

    template<typename... Args>
    void emplace(Args&&... args)
    {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.emplace(std::forward<Args>(args)...);
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
                std::pair<std::function<void()>,Attr> task;
                std::unique_lock<std::mutex> lk(pool->mtx);
                if(pool->work_queue.empty())    
                {
                    pool->running=false;
                    continue;
                }
                if(!pool->running)
                {
                    continue;
                }
                pool->cv.wait(lk,[this]{
                    return !pool->work_queue.empty();
                });
                flag=pool->work_queue.try_pop(task);
                if(flag) 
                {
                    //printf("Running task =name : [%s] ; priority: [%d] ; burst [%d] for %d units. Pid = [%d].",task.second.name.c_str(), task.second.priority, task.second.burst,task.second.burst,task.second.pid);
                    printf("Running task =name : [%s] ; priority: [%d] ; burst [%d] for %d units. Pid = [%d].\n",task.second.name.c_str(), task.second.priority, task.second.burst,task.second.burst,task.second.pid);
                    (task.first)();
                }
            }
        }
    };
    bool is_shut_down;
    bool running;
    threadsafe_priority_queue<std::pair<std::function<void()>,Attr>,std::vector<std::pair<std::function<void()>,Attr>>,Comp> work_queue;
    std::vector<std::thread> threads;
    std::mutex mtx;
    std::condition_variable cv;
public:
    threadpool(int n):threads(n),is_shut_down(false),running(false)
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

    void run()
    {
        running=true;
        //threads.clear();
    }
    

    //C++17
    template<typename F,typename... Args,
    typename=std::enable_if_t<std::is_void_v<std::invoke_result_t<std::decay_t<F>,std::decay_t<Args>...>>>>
    std::shared_future<bool> submit(std::shared_ptr<Attr> attr,F&& f,Args&&... args)
    {
        auto promise=std::make_shared<std::promise<bool>>();
        auto future=promise->get_future();
        std::function<void()> func=std::bind(std::forward<F>(f),std::forward<Args>(args)...);
        work_queue.emplace([func,promise]()
        {
            func();
            promise->set_value(true);
        },*attr);
        cv.notify_one();
        return future;
    }
    
    
    template<typename F,typename... Args,
    typename R=std::invoke_result_t<std::decay_t<F>,std::decay_t<Args>...>,
    typename=std::enable_if_t<!std::is_void_v<R>>>
    std::shared_future<R> submit(std::shared_ptr<Attr> attr,F&& f,Args&&... args)
    {
        auto promise=std::make_shared<std::promise<R>>();
        std::shared_future<R> future=promise->get_future();
        std::function<R()> func=std::bind(std::forward<F>(f),std::forward<Args>(args)...);
        work_queue.emplace([func,promise]()
        {
            promise->set_value(func());
        },*attr);
        cv.notify_one();
        return future;
    }

};

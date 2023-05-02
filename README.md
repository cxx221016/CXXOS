# CXXOS

## 目录
- [CXXOS](#cxxos)
  - [目录](#目录)
  - [摘要](#摘要)
  - [MODULE](#module)
    - [Mem module--VERSION4.3](#mem-module--version43)
    - [Func module--VERSION3.1](#func-module--version31)
    - [Threadpool module--VERSION3.1](#threadpool-module--version31)
    - [VMem module----VERSION3.1](#vmem-module----version31)
    - [BinarySerial module--VERSION4.0](#binaryserial-module--version40)
    - [Net module--VERSION4.0](#net-module--version40)
    - [mount module--VERSION5.0](#mount-module--version50)
    - [Msi module--VERSION5.0](#msi-module--version50)
  - [UPDATE](#update)
    - [VERSION 3.1](#version-31)
    - [VERSION 3.9](#version-39)
    - [VERSION 4.0](#version-40)
    - [VERSION 4.1](#version-41)
    - [VERSION 4.2](#version-42)
    - [VERSION 4.3](#version-43)
    - [VERSION 4.4](#version-44)
    - [VERSION 5.0](#version-50)
    - [VERSION 5.1](#version-51)
    - [VERSION 5.2](#version-52)
    - [VERSION 5.4](#version-54)
  - [CONFIG](#config)
    - [request](#request)
    - [CMakeLists](#cmakelists)
    - [config](#config-1)
  - [SUMMARY](#summary)
    - [2023 4.9](#2023-49)
    - [2023 4.16](#2023-416)
    - [2023 5.2](#2023-52)

## 摘要

> 大二下学习了操作系统这门课程，因此我本人对于实现一个操作系统比较感兴趣，但是无奈操作系统实现起来较为麻烦，因此我实现了一个基于C++的无GUI界面的操作系统 
> 代码量2600行

## MODULE


### Mem module--VERSION4.3

```cpp

struct File
{
    std::string path;
    std::string name;
    std::unordered_map<string,string> limits
    int address;
    int len;
    std::string data;
};

struct Dirs
{
    std::string path;
    std::string name;
    std::unordered_map<string,string> limits
    std::shared_ptr<Dirs> pre;
    std::vector<std::shared_ptr<File>> files;
    std::vector<std::shared_ptr<Dirs>> dirs;
};

class Mem
{
private:
    std::shared_ptr<Dirs> root;
    std::shared_ptr<Dirs> current;
    unsigned int  capcity;
    std::vector<std::pair<int,int>> implMem;
    std::unordered_map<std::pair<int,int>,std::shared_ptr<File>> fileMap;
    std::unordered_map<std::string,std::shared_ptr<Dirs>> dirMap;
    std::string fits;
};
```

>Mem包含三个模块:File,Dirs,Mem


### Func module--VERSION3.1
```cpp

using namespace std::string_literals;

std::unordered_map<std::string, std::function<int(int, int)>> opmap

struct fileattr
{
    int key;
    std::string time;
    std::string filename;
};

class Memtable
{
private:
    std::string filename;
    std::map<int,fileattr> memtable;
    int curkey;
};

```

>opmap :定义了常用函数，进行调度
>Memtable :记录并管理Mem的Serial文件，便于deSerial
>fileattr :是描述Mem的Serial文件的特性，包括timestamp，id，filepath

### Threadpool module--VERSION3.1
```cpp
struct Attr
{
    std::string name;
    int priority;
    int burst;
    int pid;
};

struct OutComp
{
    bool operator()()
};

struct Comp
{
    bool operator()(c)
};

//static unsigned int cnt=0;
template<typename T,
typename Sequence=std::vector<T>,
typename Compare=std::less<typename T::value_type>> 
class threadsafe_priority_queue;

template<typename T>
class threadsafe_queue;


class threadpool
{
private:
    friend class worker;
    class worker
    {
        threadpool* pool;
        void operator()()
    };
    bool is_shut_down;
    bool running;
    threadsafe_priority_queue<std::pair<std::function<void()>,Attr>,
    std::vector<std::pair<std::function<void()>,Attr>>,
    Comp> work_queue;
    std::vector<std::thread> threads;
    std::mutex mtx;
    std::condition_variable cv;
    //C++17
    template<typename F,typename... Args,
    typename=std::enable_if_t<
    std::is_void_v<
    std::invoke_result_t<
    std::decay_t<F>,std::decay_t<Args>...>>>>
    std::shared_future<bool> 
    submit(std::shared_ptr<Attr> attr,F&& f,Args&&... args)
    
    template<typename F,typename... Args,
    typename R=std::invoke_result_t<std::decay_t<F>,
    std::decay_t<Args>...>,
    typename=std::enable_if_t<!std::is_void_v<R>>>
    std::shared_future<R> 
    submit(std::shared_ptr<Attr> attr,F&& f,Args&&... args)
};
```

>Attr :进程的属性
>Comp,OutComp :比较器
>threadsafe_queue,threadsafe_priority_queue : 线程安全数据结构，用于RR，SJF，FCFS,PRIORIRY 调度算法
>worker 包含threadpool,不断获取任务，从而无需关闭线程，而是线程等待
>threadpool  线程池，包含threads，cond，mutex
>> submit 使用C++17特性，对于任意类型的函数进行推导，得到含有结果的shared_future，同时将函数封装为void()类型进行管理


### VMem module----VERSION3.1

```cpp
using Tran =std::function<std::pair<int,int>(int)>;

struct tlb
{
    int pageno;
    int frameno;
    int timestamp;
};

struct page
{
    int frameno;
    bool valid;
};

struct frame
{
    int timestamp;
    char data[framesize];
};

class VMem
{
private:
    std::vector<tlb> tlbtable;
    std::vector<page> pagetable;
    std::vector<frame> frametable;
    int pagenums,tlbnums,framenums;
    int pagesize,framesize;
    std::string filename;
    double tlbhitrate,pagefaultrate;
    int tlbhitnums,pagefaultnums;
    int cnt;
    static int timestamp;
    Tran tran;
    static std::pair<int,int> impltran(int address); 
};
```
> tlb，page,frame
> VMem :进行虚拟内存的查找过程，并进行页表替换等工作


### BinarySerial module--VERSION4.0

```cpp
class BinarySerial
{
    constexpr static std::size_t blocksize = 4;
public:
    static std::size_t getsize(const std::string& filename);
    static void BinarytoSerial(const std::string& filename, const std::string &serialdata);
    static std::string ChooseSerial(const std::string& filename);
private:
    static void cxxsha(char& c);
    static void cxxsha(char* ch, std::size_t size);
    static std::string deSerial(const std::string& filename);
    static std::string BinarydeSerial(const std::string& filename);
};
```

> 提供文件的二进制序列化以及读取，加密

### Net module--VERSION4.0
```cpp
class Net
{
private:
    bool flag ;
    std::shared_ptr<Mem> mem;
    WSADATA wsaData;
    SOCKET s_server;
    SOCKADDR_IN server_addr;
    char sendbuf[1024];
    char recvbuf[1024];
public:
    Net(std::shared_ptr<Mem> mem,const std::string& ip="127.0.0.1", int port=1234);
    ~Net();
    void run();
    bool chat();
};
```
> 提供网络交互功能

### mount module--VERSION5.0

```cpp
struct MountNode
{
    std::string path;
    std::string timestamp;
    std::string ext;
    friend std::ostream& operator<<(std::ostream& os, const MountNode& node)
    friend std::istream& operator>>(std::istream& is, MountNode& node)
};
class Mount
{
private:
    std::unordered_map<std::string, std::shared_ptr<MountNode>> mount_table;
    std::vector<std::shared_ptr<MountNode>> mount_list;
public:
    void list();
    void add();
    void add(const std::string& path);
    void remove(const std::string& path);
    void remove(int idx);
    void clear();
    void open(const std::string& path);
    void open(int idx);
    friend std::ostream& operator<<(std::ostream& os, const Mount& mount)
    friend std::istream& operator>>(std::istream& is, Mount& mount)
};
```

> 挂载外部文件，基于WIN32API实现


### Msi module--VERSION5.0

```cpp
struct MsiNode
{
    std::string name;
    std::string version;
    friend std::ostream& operator<<(std::ostream& os,const MsiNode& node)
    friend std::istream& operator>>(std::istream& is,MsiNode& node)
};

class Msi
{
    std::unordered_map<std::string,std::string> package_table;
    std::unordered_map<std::string,std::shared_ptr<MsiNode>> msi_table;
    void pipinstall(const std::string& name);
    void pipuninstall(const std::string& name);
    void piplist();
    void pipupgrade(const std::string& name);
    void pipshow(const std::string& name);
    friend std::ostream& operator<<(std::ostream& os,const Msi& msi)
    friend std::istream& operator>>(std::istream& is,Msi& msi)
};
```

> 包管理头文件

## UPDATE
### VERSION 3.1 
> 文件使用相对路径，防止bug

### VERSION 3.9
> 添加了net模块，用于chat
> mem 改为shared_ptr

### VERSION 4.0
> net 模块包括服务端和用户端，可进行文件下载和传输
> 但是由于g++和vs的不同，存在一点点bug
> VMem 模块 tlbsize和pagesize写反，同时输出进制写错了

### VERSION 4.1
> 对于存储文件采用了二进制存储，压缩空间同时保证数据安全
> 修复了网络通信的bug
> 增加内核模块，直接处理所有相关量，不再保持main函数

### VERSION 4.2
> 对Memtable进行了二进制加密，增加可靠性
> 重载了输出函数，使得结构大幅缩减


### VERSION 4.3
> 增加了文件权限
> 同时增加了文件权限相应函数
> 构建内存时出现bug，已经修复
> ren
> edit
> findfile,finddir优化
> fc
> cat


### VERSION 4.4
> 更改文件索引方式以及底层实现
> 增加相应文件操作


### VERSION 5.0
> 添加用户以及对应权限
> 对于各函数权限细化，首先通过函数判断是否具有权限
> 增加改变用户

### VERSION 5.1
> 对用户进行了更深层次的限制
> 修改了CMakeLists.txt 目前支持多平台，多编译器编译
> 对于WINDOWS,仍然可以使用net库，其它不行

### VERSION 5.2
> 增加了文件挂载功能
> 但此功能使用win32 API后，会出现文件无法保存的情况

### VERSION 5.4
> 增加了包管理功能


## CONFIG

### request
> std=c++17

### CMakeLists
```cmake
cmake_minimum_required(VERSION 3.10)
project(cxxos)
option(LOG "Enable log" OFF)
option(MOUNT "Enable mount" OFF)
option(MSI "Enable MSI" ON)
option(NET "Enable network" OFF)
option(THREADPOOL "Enable thread pool" OFF)
option(VMEM "Enable virtual memory" OFF)

if(LOG)
    add_compile_definitions(CXX_LOG)
endif()
if(MOUNT)
    add_compile_definitions(CXX_MOUNT)
endif()
if(MSI)
    add_compile_definitions(CXX_MSI)
endif()
if(NET)
    add_compile_definitions(CXX_NET)
endif()
if(THREADPOOL)
    add_compile_definitions(CXX_THREADPOOL)
endif()
if(VMEM)
    add_compile_definitions(CXX_VMEM)
endif()
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE "Release")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lws2_32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lcomdlg32")
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(Mem STATIC ${PROJECT_SOURCE_DIR}/src/Mem.cpp)
if(VMEM)
add_library(VMem STATIC ${PROJECT_SOURCE_DIR}/src/VMem.cpp)
endif()
if(WIN32)
    if(NET)
        add_library(Net STATIC ${PROJECT_SOURCE_DIR}/src/Mem.cpp ${PROJECT_SOURCE_DIR}/src/Net.cpp)
    endif()
endif()
add_library(BinarySerial STATIC ${PROJECT_SOURCE_DIR}/src/BinarySerial.cpp)
if(WIN32)
    if(MOUNT)
        add_library(Mount STATIC ${PROJECT_SOURCE_DIR}/src/BinarySerial.cpp ${PROJECT_SOURCE_DIR}/src/Mount.cpp)
    endif()
endif()
if(MSI)
add_library(Msi STATIC ${PROJECT_SOURCE_DIR}/src/BinarySerial.cpp ${PROJECT_SOURCE_DIR}/src/Msi.cpp)
endif()

add_executable(main ${PROJECT_SOURCE_DIR}/main.cpp)
target_link_libraries(main Mem  BinarySerial)
if(VMEM)
target_link_libraries(main VMem)
endif()

if(MSI)
target_link_libraries(main Msi)
endif()

if(WIN32)
    if(NET)
    target_link_libraries(main Net ws2_32)
    endif()
    if(MOUNT)
    target_link_libraries(main Mount comdlg32)
    endif()
endif()
```

对应编译选项与表格如下
|选项|宏|模块|命令|
|--|--|--|--|
|LOG|CXX_LOG|日志模块(kernel.h)|-D LOG=ON|
|MOUNT|CXX_MOUNT|挂载模块([mount.h](#mount-module--version50))|-D MOUNT=ON|
|MSI|CXX_MSI|包管理模块([msi.h](#msi-module--version50))|-D MSI=ON|
|NET|CXX_NET|网络模块([net.h](#net-module--version40))|-D NET=ON|
|THREADPOOL|CXX_THREADPOOL|线程池模块([threadpool.h](#threadpool-module--version31))|-D THREADPOOL=ON|
|VMEM|CXX_VMEM|虚拟内存模块([Vmem.h](#vmem-module----version31))|-D VMEM=ON|

使用相应的cmake选项，可添加相应模块，或直接修改CMakeLists.txt

### config
* git clone git@github.com:cxx221016/CXXOS.git
* cd OS
* for windows with mingw && cmake
   * rd /S build
   * md build
   * cd build
   * cmake -G cmake -G "MinGW Makefiles" ..
   * mingw32-make
   * cd ../bin
   * main.exe
* for windows with VS && cmake
   * rd /S build
   * md build
   * cd build
   * cmake ..
   * find *.sln
   * open *.sln
* for linux with g++ && cmake
   * rm -rf build
   * mkdir build
   * cd build
   * cmake ..
   * make 
   * cd ../bin
   * ./main


## SUMMARY
### 2023 4.9 
虽然实现了一部分功能，但是还是有很大的缺陷，很多功能没有完善，这个月的目标是实现文件挂载，同时对命令行命令进行更进一步带参数的拓展

### 2023 4.16
目前主要任务是实现更多的指令参数，例如rd /S/T等等

### 2023 5.2
此后os主要方向转换为添加类似于mount,msi之类的模块，相应的，也会cmakelists也被更改，使用了optional，预定义宏来定下模块

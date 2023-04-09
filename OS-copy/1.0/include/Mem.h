#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<unordered_map>
#include<memory>
#include<stack>

namespace std {
template<>
class hash<pair<int,int>> 
{
public:
    size_t operator()(const pair<int,int> &p) const {
        return hash<int>()(p.first)^hash<int>()(p.second);
    }
};
};

struct File
{
    std::string path;
    std::string name;
    int address;
    int len;

    std::string data;
    //File(int begin,int end,const std::string& name="CXX:"):begin(begin),end(end),name(name){}
    File(const std::string& path,const std::string& name,int address=0,int len=0,const std::string& data="cxxos")
    {
        this->path=path;
        this->name=name;
        this->data=data;
        this->address=address;
        this->len=len;
    }
};

struct Dirs
{
    std::string path;
    std::string name;
    std::shared_ptr<Dirs> pre;
    std::vector<std::shared_ptr<File>> files;
    std::vector<std::shared_ptr<Dirs>> dirs;
    //Dirs* pre;
    //std::vector<File*> files;
    //std::vector<Dirs*> dirs;
    Dirs(const std::string& path="/CXX",const std::string& name="CXX",std::shared_ptr<Dirs> pre=nullptr)
    {
        this->name=name;
        this->path=path;
        this->pre=pre;
    }
    Dirs(const Dirs&)=default;
    Dirs(Dirs&&)=default;
    Dirs& operator=(const Dirs&)=default;
    Dirs& operator=(Dirs&&)=default;
};

class Mem
{
private:
    std::shared_ptr<Dirs> root;
    std::shared_ptr<Dirs> current;
    //Dirs* root;
    //Dirs* current;
    unsigned int  capcity;
    std::vector<std::pair<int,int>> implMem;
    std::unordered_map<std::pair<int,int>,std::shared_ptr<File>> fileMap;
    std::unordered_map<std::string,std::shared_ptr<Dirs>> dirMap;
    std::string fits;
public:
    Mem(unsigned int capcity=1024,const std::string& fits="firstfit");
    Mem(const Mem&)=delete;
    Mem(Mem&&)=delete;
    Mem& operator=(const Mem&)=delete;
    Mem& operator=(Mem&&)=delete;

    //Mem
    int alloc(int len);
    void free(int address,int len);
    void compact();
    void status();
    int firstfit(int len);
    int bestfit(int len);
    int worstfit(int len);

    //File
    void md(const std::string& name); //make directory
    void rd(const std::string& name); //remove directory
    void type(const std::string &name,int len,const std::string &data="cxxos"); //type file
    void del(const std::string& name); //delete file
    void cd(const std::string& name); //change directory
    void dir(); //list directory

    //impl Mem
    unsigned int getcapcity();
    std::string getcurdir();//get current directory
    void debug(); //debug
};
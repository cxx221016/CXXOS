#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<unordered_map>
#include<memory>
#include<stack>
#include<sstream>
#include<fstream>
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
    File()=default;
    File(const std::string& path,const std::string& name,int address=0,int len=0,const std::string& data="cxxos")
    {
        this->path=path;
        this->name=name;
        this->data=data;
        this->address=address;
        this->len=len;
    }
    File(const File&)=default;
    File(File&&)=default;
    File& operator=(const File&)=default;
    File& operator=(File&&)=default;

    friend std::ostream &operator<<(std::ostream &os,const File& file)
    {
        os << "path: " << file.path << " name: " << file.name << " address: " << file.address << " len: " << file.len << " data: " << file.data ;
        return os;
    }

    friend std::istream &operator>>(std::istream &is,File& file)
    {
        std::string pass;
        //is>>pass >> file.path >> file.name >> file.address >> file.len >> file.data;
        is>>pass>>file.path>>pass>>file.name>>pass>>file.address>>pass>>file.len>>pass>>file.data;
        return is;
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
    ~Dirs()=default;
    friend std::ostream &operator<<(std::ostream &os,const Dirs& dir)
    {
        //os << "path: " << path << " name: " << name ;
        os<<"path: "<<dir.path<<" name: "<<dir.name;
        return os;
    }
};

class Net;

class Mem
{
private:
    friend class Net;
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

    std::vector<std::shared_ptr<File>> SearchFile(const std::string &name);
    std::vector<std::shared_ptr<Dirs>> SearchDir(const std::string &name);
    std::shared_ptr<File> findFile(const std::string& path,const std::string& name);
    std::shared_ptr<Dirs> findDir(const std::string& path);
    void addFile(const std::string& path,const File& file);
    void addFile(const std::string& path,std::shared_ptr<File> file);
    void addFile(const std::string& path,const std::string& name,int len,const std::string& data="cxxos");
    void addDir(const std::string& path,const Dirs& dir);
    void addDir(const std::string& path,std::shared_ptr<Dirs> dir);
    void addDir(const std::string& path,const std::string& name);
    void eraseFile(std::shared_ptr<File> file);
    void eraseDir(std::shared_ptr<Dirs> dir);
    void md(const std::string& name); //make directory
    void rd(const std::string& name); //remove directory
    void type(const std::string &name,int len,const std::string &data="cxxos"); //type file
    void del(const std::string& name); //delete file
    void cd(const std::string& name); //change directory
    void copy(const std::string& src,const std::string& dst); //copy file
    void dir(); //list directory

    //impl Mem
    unsigned int getcapcity();
    std::string getcurdir();//get current directory
    void debug(); //debug
    static void toSerial(const std::string& path,const Mem& mem);
    static bool deSerial(const std::string& path,Mem& mem);

    //Serial
    friend std::ostream& operator<<(std::ostream& os,const Mem& mem);
    friend std::istream& operator>>(std::istream& os,Mem& mem);
};
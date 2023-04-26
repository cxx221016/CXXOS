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
    std::unordered_map<std::string,std::string> limits;
    std::string data;
    //File(int begin,int end,const std::string& name="CXX:"):begin(begin),end(end),name(name){}
    File()=default;
    File(const std::string& path,const std::string& name,int address=0,int len=0,const std::string& data="cxxos",const std::string& usr="root")
    {
        this->path=path;
        this->name=name;
        this->data=data;
        this->address=address;
        this->len=len;
        this->limits["root"]="-rwx";
        if(usr!="root")
            this->limits[usr]="-rwx";
    }
    File(const File&)=default;
    File(File&&)=default;
    File& operator=(const File&)=default;
    File& operator=(File&&)=default;

    friend std::ostream &operator<<(std::ostream &os,const File& file)
    {
        //os << "path: " << file.path << " name: " << file.name << " address: " << file.address << " len: " << file.len << " data: " << file.data << " limits: " << file.limits;
        char buf[4096];
        sprintf(buf,"path: %-20s name: %-20s address: %-20d len: %-20d data: %-20s ",file.path.c_str(),file.name.c_str(),file.address,file.len,file.data.c_str());
        os<<buf;
        for(auto& i:file.limits)
        {
            os<<i.first<<" "<<i.second<<" ";
        }
        os<<std::endl;
        return os;
    }

    friend std::istream &operator>>(std::istream &is,File& file)
    {
        std::string pass;
        //is>>pass >> file.path >> file.name >> file.address >> file.len >> file.data;
        is>>pass>>file.path>>pass>>file.name>>pass>>file.address>>pass>>file.len>>pass>>file.data;
        std::string usrname,limit;
        while(is>>usrname>>limit)
        {
            file.limits[usrname]=limit;
        }
        return is;
    }
};

struct Dirs
{
    std::string path;
    std::string name;
    std::unordered_map<std::string,std::string> limits;
    std::shared_ptr<Dirs> pre;
    std::vector<std::shared_ptr<File>> files;
    std::vector<std::shared_ptr<Dirs>> dirs;
    //Dirs* pre;
    //std::vector<File*> files;
    //std::vector<Dirs*> dirs;
    Dirs(const std::string& path="/CXX",const std::string& name="CXX",std::shared_ptr<Dirs> pre=nullptr,const std::string& usr="root")
    {
        this->path=path;
        this->name=name;
        this->pre=pre;
        this->limits["root"]="-rwx";
        if(usr!="root")
            this->limits[usr]="-rwx";
    }
    Dirs(const Dirs&)=default;
    Dirs(Dirs&&)=default;
    Dirs& operator=(const Dirs&)=default;
    Dirs& operator=(Dirs&&)=default;
    ~Dirs()=default;
    friend std::ostream &operator<<(std::ostream &os,const Dirs& dir)
    {
        //os << "path: " << path << " name: " << name ;
        //os<<"path: "<<dir.path<<" name: "<<dir.name<<" limits: "<<dir.limits<<std::endl;
        char buf[4096];
        sprintf(buf,"path: %-20s name: %-20s ",dir.path.c_str(),dir.name.c_str());
        os<<buf;
        for(auto& i:dir.limits)
        {
            os<<i.first<<" "<<i.second<<" ";
        }
        os<<std::endl;
        return os;
    }

    friend std::istream& operator>>(std::istream& is,Dirs& dir)
    {
        std::string pass;
        is>>pass>>dir.path>>pass>>dir.name;
        std::string usrname,limit;
        while(is>>usrname>>limit)
        {
            dir.limits[usrname]=limit;
        }
        return is;
    }
};

static std::string innerdata="cxxos";
static std::string innerfits="firstfit";

class Net;
class kernel;

class Mem
{
private:
    friend class Net;
    friend class kernel;
    std::shared_ptr<Dirs> root;
    std::shared_ptr<Dirs> current;
    //Dirs* root;
    //Dirs* current;
    unsigned int  capacity;
    std::vector<std::pair<int,int>> implMem;
    std::unordered_map<std::pair<int,int>,std::shared_ptr<File>> fileMap;
    std::unordered_map<std::string,std::shared_ptr<Dirs>> dirMap;
    std::string fits;
    std::string usrname;
    std::unordered_map<std::string,std::string> usrMap;
private:
    //Mem
    int alloc(int len);
    void free(int address,int len);
    int firstfit(int len);
    int bestfit(int len);
    int worstfit(int len);

    //File
    std::pair<std::string,std::string> filesplitPath(const std::string& init);
    std::pair<std::string,std::string> dirsplitPath(const std::string& init);
    void eraseFile(std::shared_ptr<File> file);//delete file
    void eraseDir(std::shared_ptr<Dirs> dir);//delete dir and all files in it
    void implmd(std::shared_ptr<Dirs> curdir,const std::string& name); //make directory
    void implrd(std::shared_ptr<Dirs> curdir,const std::string& name); //remove directory
    void impldel(std::shared_ptr<Dirs> curdir,const std::string& name); //delete file
    template<typename T>
    void implchmod(T t,const std::vector<std::string>& args);

    template<typename T>
    bool hasPermission(T t,const std::string& funcusrname);

    template<typename T>
    bool hasPermission(T t,const std::string& funcusrname,const char& limit);
public:
    Mem(unsigned int capacity=1024,const std::string& fits=innerfits);
    Mem(const Mem&)=delete;
    Mem(Mem&&)=delete;
    Mem& operator=(const Mem&)=delete;
    Mem& operator=(Mem&&)=delete;

    //system
    bool login(const std::string& funcusrname,const std::string& password);
    void logout();
    void addusr(const std::string& funcusrname,const std::string& password);
    void delusr(const std::string& funcusrname);

    //Mem
    void compact();
    void status();

    //File

    std::vector<std::shared_ptr<File>> SearchFile(const std::string &name);//search file
    std::vector<std::shared_ptr<Dirs>> SearchDir(const std::string &name);//search dir
    std::shared_ptr<File> findFile(const std::string& init);//find file
    //std::shared_ptr<File> findFile(const std::string& path,const std::string& name);//find file
    std::shared_ptr<Dirs> findDir(const std::string& init);//find dir
    //void addFile(const std::string& path,const std::string& name,int len=12,const std::string& data="cxxos");//add file
    //void addDir(const std::string& path,const std::string& name);//add dir
    void md(const std::string& init); //make directory
    void rd(const std::string& init); //remove directory
    void impltype(std::shared_ptr<Dirs> curdir,const std::string &name,int len=12,const std::string &data=innerdata); //type file
    void type(const std::string &init,int len=12,const std::string &data=innerdata); //type file
    void del(const std::string& init); //delete file
    void ren(const std::string& args,std::string newname); //rename file
    void edit(const std::string& args);//edit file
    void fc(const std::string& args1,const std::string& args2); //file compare
    void cat(const std::string& args); //cat file
    void chmod(const std::string& init,const std::vector<std::string>& args); //chmod (file or dir)
    void showmod(const std::string& init); //show mod
    void chmon(const std::string& init,const std::string& toaddname,bool flag=true); //add user
    void cd(const std::string& name); //change directory
    void copy(const std::string& src,const std::string& dst); //copy file
    void move(const std::string& src,const std::string& dst); //move file
    void dir(); //list directory

    //impl Mem
    unsigned int getcapacity();
    std::string getcurdir();//get current directory
    void debug(); //debug
    static void toSerial(const std::string& path,const Mem& mem);
    static bool deSerial(const std::string& path,Mem& mem);

    //Serial
    friend std::ostream& operator<<(std::ostream& os,const Mem& mem);
    friend std::istream& operator>>(std::istream& os,Mem& mem);
};


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
    std::string  limits;
    std::string data;
    //File(int begin,int end,const std::string& name="CXX:"):begin(begin),end(end),name(name){}
    File()=default;
    File(const std::string& path,const std::string& name,int address=0,int len=0,const std::string& data="cxxos",const std::string& limits="-rw-")
    {
        this->path=path;
        this->name=name;
        this->data=data;
        this->address=address;
        this->len=len;
        this->limits=limits;
    }
    File(const File&)=default;
    File(File&&)=default;
    File& operator=(const File&)=default;
    File& operator=(File&&)=default;

    friend std::ostream &operator<<(std::ostream &os,const File& file)
    {
        //os << "path: " << file.path << " name: " << file.name << " address: " << file.address << " len: " << file.len << " data: " << file.data << " limits: " << file.limits;
        char buf[4096];
        sprintf(buf,"path: %-20s name: %-20s address: %-20d len: %-20d data: %-20s limits: %-5s\n",file.path.c_str(),file.name.c_str(),file.address,file.len,file.data.c_str(),file.limits.c_str());
        os<<buf;
        return os;
    }

    friend std::istream &operator>>(std::istream &is,File& file)
    {
        std::string pass;
        //is>>pass >> file.path >> file.name >> file.address >> file.len >> file.data;
        is>>pass>>file.path>>pass>>file.name>>pass>>file.address>>pass>>file.len>>pass>>file.data>>pass>>file.limits;
        return is;
    }
};

struct Dirs
{
    std::string path;
    std::string name;
    std::string limits;
    std::shared_ptr<Dirs> pre;
    std::vector<std::shared_ptr<File>> files;
    std::vector<std::shared_ptr<Dirs>> dirs;
    //Dirs* pre;
    //std::vector<File*> files;
    //std::vector<Dirs*> dirs;
    Dirs(const std::string& path="/CXX",const std::string& name="CXX",std::shared_ptr<Dirs> pre=nullptr,const std::string& limits="drw-")
    {
        this->name=name;
        this->path=path;
        this->pre=pre;
        this->limits=limits;
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
        sprintf(buf,"path: %-20s name: %-20s limits: %-20s\n",dir.path.c_str(),dir.name.c_str(),dir.limits.c_str());
        os<<buf;
        return os;
    }

    friend std::istream& operator>>(std::istream& is,Dirs& dir)
    {
        std::string pass;
        is>>pass>>dir.path>>pass>>dir.name>>pass>>dir.limits;
        return is;
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
private:
    //Mem
    int alloc(int len);
    void free(int address,int len);
    int firstfit(int len);
    int bestfit(int len);
    int worstfit(int len);

    //File

    std::shared_ptr<File> findFile(const std::string& name);
    std::shared_ptr<File> findFile(const std::string& path,const std::string& name);
    std::shared_ptr<Dirs> findDir(const std::string& path);
    std::shared_ptr<Dirs> findDir(const std::string& path,const std::string& name);
    void addFile(const std::string& path,const File& file);
    void addFile(const std::string& path,std::shared_ptr<File> file);
    void addDir(const std::string& path,const Dirs& dir);
    void addDir(const std::string& path,std::shared_ptr<Dirs> dir);

    template<typename T>
    void implchmod(T t,std::vector<std::string>& args);
public:
    Mem(unsigned int capcity=1024,const std::string& fits="firstfit");
    Mem(const Mem&)=delete;
    Mem(Mem&&)=delete;
    Mem& operator=(const Mem&)=delete;
    Mem& operator=(Mem&&)=delete;

    //Mem
    void compact();
    void status();

    //File

    std::vector<std::shared_ptr<File>> SearchFile(const std::string &name);//search file
    std::vector<std::shared_ptr<Dirs>> SearchDir(const std::string &name);//search dir
    std::shared_ptr<File> findFile(const std::vector<std::string>& args);//find file
    std::shared_ptr<Dirs> findDir(const std::vector<std::string>& args);//find dir
    void addFile(const std::string& path,const std::string& name,int len,const std::string& data="cxxos");//add file
    void addDir(const std::string& path,const std::string& name);//add dir
    void eraseFile(std::shared_ptr<File> file);//delete file
    void eraseDir(std::shared_ptr<Dirs> dir);//delete dir and all files in it
    void md(const std::string& name); //make directory
    void rd(const std::string& name); //remove directory
    void type(const std::string &name,int len=12,const std::string &data="cxxos"); //type file
    void del(const std::string& name); //delete file
    void ren(const std::vector<std::string>& args,std::string newname); //rename file
    void edit(const std::vector<std::string>& args);//edit file
    void fc(const std::vector<std::string>& args1,const std::vector<std::string>& args2); //file compare
    void cat(const std::vector<std::string>& args); //cat file
    void chmod(const std::string& path,std::vector<std::string>& args); //change file mode
    void chmod(const std::string& path,const std::string& name,std::vector<std::string>& args); //change file mode
    void showmod(const std::string& path,const std::string& name); //show file mode
    void showmod(const std::string& path); //show file mode
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
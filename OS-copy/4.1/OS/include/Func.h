#pragma
#include <unordered_map>
#include<map>
#include <functional>
#include <string>
#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include<chrono>
#include<iomanip>
#include<optional>

using namespace std::string_literals;

const std::string constbase="..\\file\\Mem";
const std::string constext=".bin";

std::unordered_map<std::string, std::function<int(int, int)>> opmap{
    {"add"s, [](int a, int b)
     { return a + b; }},
    {"sub"s, [](int a, int b)
     { return a - b; }},
    {"mul"s, [](int a, int b)
     { return a * b; }},
    {"div"s, [](int a, int b)
     { return a / b; }},
    {"mod"s, [](int a, int b)
     { return a % b; }},
    {"and"s, [](int a, int b)
     { return a & b; }},
    {"or"s, [](int a, int b)
     { return a | b; }},
    {"xor"s, [](int a, int b)
     { return a ^ b; }},
    {"not"s, [](int a, int b)
     { return ~a; }},
    {"shl"s, [](int a, int b)
     { return a << b; }},
    {"shr"s, [](int a, int b)
     { return a >> b; }},
    {"eq"s, [](int a, int b)
     { return a == b; }},
    {"neq"s, [](int a, int b)
     { return a != b; }},
    {"less"s, [](int a, int b)
     { return a < b; }},
    {"leq"s, [](int a, int b)
     { return a <= b; }},
    {"great"s, [](int a, int b)
     { return a > b; }},
    {"geq"s, [](int a, int b)
     { return a >= b; }},
    {"hello"s, [](int a, int b)
     {std::cout<<"Hello World!";return 0; }}};


struct fileattr
{
    int key;
    std::string time;
    std::string filename;
    fileattr()=default;
    fileattr(int key,std::string time,std::string filename):key(key),time(time),filename(filename){}
    fileattr(const fileattr&)=default;
    fileattr(fileattr&&)=default;
    fileattr& operator=(const fileattr&)=default;
    fileattr& operator=(fileattr&&)=default;
    friend std::ostream& operator<<(std::ostream& os,const fileattr& fa)
    {
        os<<fa.key<<" "<<fa.time<<" "<<fa.filename;
        return os;
    }
    friend std::istream& operator>>(std::istream& is,fileattr& fa)
    {
        std::string first,second;
        is>>fa.key>>first>>second>>fa.filename;
        fa.time=first+" "+second;
        
        return is;
    }
};

class Memtable
{
private:
    std::string filename;
    std::map<int,fileattr> memtable;
    std::string base;
    std::string ext;
    int curkey;
    void compact()
    {
        int idx=0;
        std::map<int,fileattr> tmp;
        for(auto it=memtable.begin();it!=memtable.end();it++)
        {
            auto attr=it->second;
            attr.key=idx++;
            tmp[attr.key]=attr;
        }
        memtable.clear();
        memtable=std::move(tmp);
        std::ofstream fout(filename,std::ios_base::out);
        for(auto& [idx,attr]:memtable)
        {
            fout<<attr<<std::endl;
        }
    }
public:
    Memtable(const std::string &filename = "..\\file\\Memtable.txt",const std::string& base=constbase,const std::string& ext=constext) : filename(filename)
    {
        this->base=base;
        this->ext=ext;
        std::ifstream fin;
        fin.open(filename);
        if (!fin.is_open())
        {
            std::cout << "Error opening file";
            exit(1);
        }
        /*
        fout.open(filename, std::ios::app);
        if (!fout.is_open())
        {
            std::cout << "Error opening file";
            exit(1);
        }*/
        std::string line;
        while (std::getline(fin, line))
        {
            fileattr fa;
            std::istringstream iss(line);
            iss >> fa;
            std::ifstream tmpfin(fa.filename);
            if (!tmpfin.is_open())
            {
                std::cout<<"file not exist: "<<fa.filename<<std::endl;
                continue;
            }
            tmpfin.close();
            memtable[fa.key]=fa;
        }
        compact();
        curkey=memtable.size();
    }
    ~Memtable()
    {
        compact();
    }
    Memtable(const Memtable &) = delete;
    Memtable &operator=(const Memtable &) = delete;
    Memtable(Memtable &&) = delete;
    Memtable &operator=(Memtable &&) = delete;

    std::string generate()
    {
        //std::ofstream fout(filename, std::ios::app);
        //std::string base = "..\\file\\Mem";
        //std::string ext = ".txt";
        std::time_t t = std::time(nullptr);
        auto tm_t = std::localtime(&t);
        std::string time = std::to_string(tm_t->tm_year + 1900) + std::to_string(tm_t->tm_mon + 1) + std::to_string(tm_t->tm_mday) + "-" + std::to_string(tm_t->tm_hour) + std::to_string(tm_t->tm_min) + std::to_string(tm_t->tm_sec);
        // std::string time="1:1";
        std::string thisfilename = base + time + ext;
        int thiskey=curkey++;
        std::stringstream ss;
        ss<<std::put_time(tm_t,"%Y-%m-%d %H:%M:%S");
        std::string thisfiletime=ss.str();
        fileattr fa(thiskey,thisfiletime,thisfilename);
        memtable[thiskey]=fa;
        //fout<<fa<<std::endl;
        std::cout<<fa<<std::endl;
        return thisfilename;
    }

    std::string generate(std::string time)
    {
        //std::string base = "..\\file\\Mem";
        //std::string ext = ".txt";
        // std::time_t t=std::time(nullptr);
        // std::string time=std::to_string(t);
        return base + time + ext;
    }

    void erase(int idx)
    {
        auto attr=memtable[idx];
        auto res=remove(attr.filename.c_str());
        if(!res) 
        {
            std::cout<<"erase: "<<attr<<std::endl;
        }
        memtable.erase(idx);
        compact();
    }

    void show()
    {
        for(auto& [idx,attr]:memtable)
        {
            std::cout<<attr<<std::endl;
        }
    }

    std::optional<fileattr> operator[](int idx)
    {
        if(idx>=memtable.size()) 
        {
            std::cout<<"out of range: "<<std::endl;
            return  std::nullopt;
        }
        return memtable[idx];
    }
};

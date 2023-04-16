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
#include<functional>

/*
const unsigned  int pagenums=256;
const unsigned int tlbnums=16;
const unsigned int framenums=256;
const unsigned int pagesize=256;
*/
const unsigned int framesize=256;
//framesize*framenums<=65536=2^16

using Tran =std::function<std::pair<int,int>(int)>;

struct tlb
{
    int pageno;
    int frameno;
    int timestamp;
    //tlb()=default;
    tlb(int pageno=-1,int frameno=-1,int timestamp=-1):pageno(pageno),frameno(frameno),timestamp(timestamp){}
    tlb(const tlb&)=default;
    tlb(tlb&&)=default;
    tlb& operator=(const tlb&)=default;
    tlb& operator=(tlb&&)=default;
};

struct page
{
    int frameno;
    bool valid;
    page(int frameno=-1,bool valid=false):frameno(frameno),valid(valid){}
    page(const page&)=default;
    page(page&&)=default;
    page& operator=(const page&)=default;
    page& operator=(page&&)=default;
};

struct frame
{
    int timestamp;
    char data[framesize];
    frame(int timestamp=-1):timestamp(timestamp){}
    frame(const frame&)=default;
    frame(frame&&)=default;
    frame& operator=(const frame&)=default;
    frame& operator=(frame&&)=default;
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
public:
    //VMem(int pagenums=16,int tlbnums=256,int framenums=256,int pagesize=256,int framesize=256,const std::string& filename="D:\\MY_CODE\\CXXOS\\OS\\file\\VMem.bin",Tran tran=impltran);
    VMem(int pagenums=256,int tlbnums=16,int framenums=256,int pagesize=256,int framesize=256,const std::string& filename="..\\file\\VMem.bin",Tran tran=impltran);
    ~VMem();
    void status();
    void reset();
    void rptlb(int pageno,int frameno);
    void rppage(int pageno,int frameno);
    void get(int address);
    
};
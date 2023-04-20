#include "../include/VMem.h"

int VMem::timestamp=0;

VMem::VMem(int pagenums,int tlbnums,int framenums,int pagesize,int framesize,const std::string& filename,Tran tran )
{
    std::ifstream fin(filename,std::ios::binary);
    tlbtable.resize(tlbnums);
    pagetable.resize(pagenums);
    frametable.resize(framenums);
    this->filename=filename;
    this->pagenums=pagenums;
    this->tlbnums=tlbnums;
    this->framenums=framenums;
    this->pagesize=pagesize;
    this->framesize=framesize;
    this->tran=tran;
    tlbhitrate=0;
    pagefaultrate=0;
    tlbhitnums=0;
    pagefaultnums=0;
    cnt=0;
}

VMem::~VMem()
{

}

std::pair<int,int> VMem::impltran(int address)
{
    int pageno=(address>>8)&0xff;
    int offset=address&0xff;
    return std::make_pair(pageno,offset);
}

void VMem::get(int address)
{
    timestamp++;
    cnt++;
    auto res=tran(address);
    int pageno=res.first;
    int offset=res.second;
    bool pagefault=true;
    bool tlbhit=false;
    int frameno;
    // tlbhit
    for(int i=0;i<tlbtable.size();i++)
    {
        if(tlbtable[i].pageno==pageno)
        {
            tlbhitnums++;
            tlbhit=true;
            pagefault=false;
            tlbtable[i].timestamp=timestamp;
            frameno=tlbtable[i].frameno;
            frametable[frameno].timestamp=timestamp;
            break;
        }

    }

    // tlb miss and page hit
    if(!tlbhit&& pagetable[pageno].valid)
    {
        pagefault=false;
        frameno=pagetable[pageno].frameno;
        rptlb(pageno,frameno);
        frametable[frameno].timestamp=timestamp;
    }

    // tlb miss and page miss
    if(!tlbhit&&pagefault)
    {

        pagefaultnums++;
        //int minframeno=0;
        for(int i=0;i<frametable.size();i++)
        {
            if(frametable[i].timestamp<frametable[frameno].timestamp)
            {
                frameno=i;
            }
        }
        rppage(pageno,frameno);
        rptlb(pageno,frameno);
    }
    int data=frametable[frameno].data[offset];
    std::cout<<"Virtual address: "<<std::hex<<address<<" Physical address: "<<std::hex<<frameno*framesize+offset<<" Value: "<<std::oct<<data<<'\n';
}

void VMem::rptlb(int pageno,int frameno)
{
    int minindex=0;
    for(int i=0;i<tlbtable.size();i++)
    {
        if(tlbtable[i].timestamp<tlbtable[minindex].timestamp)
        {
            minindex=i;
        }
    }
    tlbtable[minindex].pageno=pageno;
    tlbtable[minindex].frameno=frameno;
    tlbtable[minindex].timestamp=timestamp;
}

void VMem::rppage(int pageno,int frameno)
{
    for(int i=0;i<pagetable.size();i++)
    {
        if(pagetable[i].frameno==frameno)
        {
            pagetable[i].valid=false;
            break;
        }
    }
    std::ifstream fin(filename,std::ios::binary);
    fin.seekg(pageno*pagesize,std::ios::beg);
    fin.read((char*)frametable[frameno].data,framesize);
    frametable[frameno].timestamp=timestamp;
    pagetable[pageno].frameno=frameno;
    pagetable[pageno].valid=true;
    //frametable[frameno].timestamp=timestamp;
    //frametable[frameno].data;
}

void VMem::status()
{
    std::cout<<"tlb hit rate: "<<tlbhitrate<<' ';
    std::cout<<"page fault rate: "<<pagefaultrate<<'\n';
}

void VMem::reset()
{
    tlbhitrate=0;
    pagefaultrate=0;
    tlbhitnums=0;
    pagefaultnums=0;
    cnt=0;
}


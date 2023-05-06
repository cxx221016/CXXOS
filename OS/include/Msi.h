#pragma once
//#define CXX_MSI
#ifdef CXX_MSI
#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<unordered_map>
#include<memory>
#include<stack>
#include<sstream>
#include<fstream>
#include"BinarySerial.h"

struct MsiNode
{
    std::string name;
    std::string version;
    MsiNode(const std::string& name,const std::string& version)
        :name(name),version(version)
    {}
    MsiNode()=default;
    MsiNode(const MsiNode&)=default;
    MsiNode(MsiNode&&)=default;
    MsiNode& operator=(const MsiNode&)=default;
    MsiNode& operator=(MsiNode&&)=default;
    ~MsiNode()=default;
    friend std::ostream& operator<<(std::ostream& os,const MsiNode& node)
    {
        os<<node.name<<' '<<node.version;
        return os;
    }

    friend std::istream& operator>>(std::istream& is,MsiNode& node)
    {
        is>>node.name>>node.version;
        return is;
    }
};

static std::string constmsifilename="..\\file\\Msi.bin";
static std::string constpackagefilename="..\\file\\package.txt";

class Msi
{
private:
    std::unordered_map<std::string,std::string> package_table;
    std::unordered_map<std::string,std::shared_ptr<MsiNode>> msi_table;
    //std::shared_ptr<MsiNode> findNode(const std::string& name);
    //void pipupgrade(std::shared_ptr<MsiNode> node);
public:
    Msi(const std::string& filename=constmsifilename,const std::string& packagefilename=constpackagefilename);
    Msi(const Msi&)=delete;
    Msi(Msi&&)=delete;
    Msi& operator=(const Msi&)=delete;
    Msi& operator=(Msi&&)=delete;
    ~Msi();
    void pipinstall(const std::string& name);
    void pipuninstall(const std::string& name);
    void piplist();
    void pipupgrade(const std::string& name);
    void pipshow(const std::string& name);
    friend std::ostream& operator<<(std::ostream& os,const Msi& msi)
    {
        for(const auto& [name,node]:msi.msi_table)
        {
            os<<*node<<'\n';
        }
        return os;
    }
    friend std::istream& operator>>(std::istream& is,Msi& msi)
    {
        MsiNode node;
        while(is>>node)
        {
            msi.msi_table[node.name]=std::make_shared<MsiNode>(node);
        }
        return is;
    }
};

#endif
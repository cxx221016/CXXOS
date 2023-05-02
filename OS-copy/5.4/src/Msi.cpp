#include "../include/Msi.h"
#include "../include/BinarySerial.h"

#ifdef CXX_MSI
Msi::Msi(const std::string& filename,const std::string& packagefilename)
{
    std::string tmpforpackages=BinarySerial::ChooseSerial(packagefilename);
    std::stringstream ss1(tmpforpackages);
    std::string name,version;
    while(ss1>>name>>version)
    {
        package_table[name]=version;
    }
    std::string tmpformsi=BinarySerial::ChooseSerial(filename);
    std::stringstream ss(tmpformsi);
    ss>>*this;
}

Msi::~Msi()
{
    std::stringstream ss;
    ss<<*this;
    //std::cout<<ss.str()<<std::endl;
    BinarySerial::BinarytoSerial(constmsifilename,ss.str());
}
/*
std::shared_ptr<MsiNode> Msi::findNode(const std::string& name)
{
    if(msi_table.find(name)==msi_table.end())
    {
        std::cout<<"No such package: "<<name<<std::endl;
        return nullptr;
    }
    return msi_table[name];
}*/

void Msi::pipinstall(const std::string& name)
{
    if(package_table.find(name)==package_table.end())
    {
        std::cout<<"No such package: "<<name<<std::endl;
        return;
    }
    if(msi_table.find(name)!=msi_table.end())
    {
        std::cout<<"Package "<<name<<" "<<msi_table[name]->version<<" is already installed"<<std::endl;
        if(msi_table[name]->version!=package_table[name])
        {
            std::cout<<"lastest version is "<<package_table[name]<<std::endl;
            std::cout<<"you can use pip upgrade "<<name<<" to upgrade"<<std::endl;
        }
        return;
    }
    std::string version=package_table[name];
    std::shared_ptr<MsiNode> node=std::make_shared<MsiNode>(name,version);
    msi_table[name]=node;
    std::cout<<"Successfully installed "<<name<<' '<<version<<std::endl;
}

void Msi::pipuninstall(const std::string& name)
{
    if(msi_table.find(name)==msi_table.end())
    {
        std::cout<<"No such package: "<<name<<std::endl;
        return;
    }
    std::cout<<"Uninstalling "<<name<<' '<<msi_table[name]->version<<std::endl;
    msi_table.erase(name);
    std::cout<<"Successfully uninstalled "<<name<<std::endl;
}

void Msi::piplist()
{
    for(const auto& [name,node]:msi_table)
    {
        std::cout<<*node<<std::endl;
    }
}

void Msi::pipupgrade(const std::string& name)
{
    if(package_table.find(name)==package_table.end())
    {
        std::cout<<"No such package: "<<name<<std::endl;
        return;
    }
    if(msi_table.find(name)==msi_table.end())
    {
        std::cout<<"has not installed "<<name<<std::endl;
        return;
    }
    if(msi_table[name]->version==package_table[name])
    {
        std::cout<<"has already installed the lastest version of "<<name<<std::endl;
        return;
    }
    std::cout<<"Upgrading "<<name<<' '<<msi_table[name]->version<<" to "<<package_table[name]<<std::endl;
    msi_table[name]->version=package_table[name];
    std::cout<<"Successfully upgraded "<<name<<std::endl;

}

void Msi::pipshow(const std::string& name)
{
    if(msi_table.find(name)==msi_table.end())
    {
        std::cout<<"No such package: "<<name<<std::endl;
        return;
    }
    std::cout<<*msi_table[name]<<std::endl;
}

#endif // CXX_MSI


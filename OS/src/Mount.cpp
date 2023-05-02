#include "../include/Mount.h"
#ifdef CXX_MOUNT
#undef UNICODE
Mount::Mount()
{
    std::string tmpformount=BinarySerial::ChooseSerial(constmountfilename);
    std::stringstream ss(tmpformount);
    ss>>*this;
}

void Mount::list()
{
    for(int i=0;i<mount_table.size();i++)
    {
        char buf[256];
        sprintf(buf,"idx: [%d] %-60s %-20s",i,mount_list[i]->path.c_str(),mount_list[i]->timestamp.c_str());
        std::cout<<buf<<std::endl;
    }
}
void Mount::add()
{
	TCHAR szBuffer[MAX_PATH] = { 0 };
	OPENFILENAME file = { 0 };
	file.hwndOwner = NULL;
	file.lStructSize = sizeof(file);
	//file.lpstrFilter = "所有文件(*.*)\0*.*\0Exe文件(*.exe)\0*.exe\0";//要选择的文件后缀 
    file.lpstrFilter = "file(*.*)";
	file.lpstrInitialDir = "";//默认的文件路径 
	file.lpstrFile = szBuffer;//存放文件的缓冲区 
	file.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
	file.nFilterIndex = 0;
	file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;//标志如果是多选要加上OFN_ALLOWMULTISELECT
	BOOL bSel = GetOpenFileName(&file);
    if(!bSel)
    {
        return;
    }
    std::string path{file.lpstrFile};
    add(path);
}

void Mount::add(const std::string& path)
{
    if(mount_table.find(path)!=mount_table.end())
    {
        mount_table[path]->timestamp=gettimestamp();
        return;
    }
    else 
    {
        mount_table[path]=std::make_shared<MountNode>(path);
        mount_list.push_back(mount_table[path]);
    }
}

void Mount::remove(const std::string& path)
{
    if(mount_table.find(path)!=mount_table.end())
    {
        auto file=mount_table[path];
        file=nullptr;
        mount_table.erase(path);
    }
}

void Mount::remove(int idx)
{
    if(idx>=mount_list.size())
    {
        return;
    }
    auto file=mount_list[idx];
    mount_table.erase(file->path);
    file=nullptr;
}

void Mount::clear()
{
    mount_table.clear();
    mount_list.clear();
}

void Mount::open(const std::string& path)
{
    if(mount_table.find(path)==mount_table.end())
    {
        return;
    }
    auto file=mount_table[path];
    if(open_table.find(file->ext)==open_table.end())
    {
        open_table[file->ext]="notepad ";
    }
    std::string cmd=open_table[file->ext];
    if(file->ext=="exe") cmd+="\"\"";
    cmd+="  \""+file->path+"\"";
    system(cmd.c_str());
}

void Mount::open(int idx)
{
    if(idx>=mount_list.size())
    {
        return;
    }
    auto file=mount_list[idx];
    if(file==nullptr)
    {
        return;
    }
    if(open_table.find(file->ext)==open_table.end())
    {
        open_table[file->ext]="notepad ";
    }
    std::string cmd=open_table[file->ext];
    if(file->ext=="exe") cmd+="\"\"";
    cmd+="  \""+file->path+"\"";
    system(cmd.c_str());
}

Mount::~Mount()
{
    std::stringstream ss;
    ss<<*this;
    //std::cout<<ss.str()<<std::endl;
    BinarySerial::BinarytoSerial(constmountfilename,ss.str());
}
#define UNICODE
#endif

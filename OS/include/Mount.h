#pragma once
#ifdef CXX_MOUNT
#ifdef _WIN32
#undef UNICODE
#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<unordered_map>
#include<memory>
#include<stack>
#include<sstream>
#include<fstream>
#include<ctime>
#include<numeric>
#include<iomanip>
#include<windows.h>
#include <commdlg.h>
#include"BinarySerial.h"

static std::unordered_map<std::string, std::string> open_table
{
    {"txt","notepad "},
    {"cpp","code "},
    {"exe","start "}
};

static std::string constmountfilename="..\\file\\Mount.bin";

static std::string gettimestamp()
{
    std::stringstream ss;
    time_t t = time(nullptr);
    auto tm = *localtime(&t);
    ss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");
    return ss.str();
}

struct MountNode
{
    std::string path;
    std::string timestamp;
    std::string ext;
    MountNode(const std::string& path)
        :path(path)
    {
        auto pos = path.find_last_of('.');
        if (pos != std::string::npos)
        {
            ext = path.substr(pos + 1);
        }
        else
        {
            ext = "";
        }
        timestamp = gettimestamp();
    }
    MountNode()=default;
    MountNode(const MountNode&)=default;
    MountNode(MountNode&&)=default;
    MountNode& operator=(const MountNode&)=default;
    MountNode& operator=(MountNode&&)=default;
    ~MountNode()=default;

    friend std::ostream& operator<<(std::ostream& os, const MountNode& node)
    {
        os << node.path <<"\n";
        os << node.timestamp <<"\n";
        os << node.ext <<"\n";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, MountNode& node)
    {
        std::getline(is, node.path);
        std::getline(is, node.timestamp);
        std::getline(is, node.ext);
        return is;
    }
};

class Mount
{
private:
    std::unordered_map<std::string, std::shared_ptr<MountNode>> mount_table;
    std::vector<std::shared_ptr<MountNode>> mount_list;
public:
    Mount();
    Mount(const Mount&)=default;
    Mount(Mount&&)=default;
    Mount& operator=(const Mount&)=default;
    Mount& operator=(Mount&&)=default;
    ~Mount();
    void list();
    void add();
    void add(const std::string& path);
    void remove(const std::string& path);
    void remove(int idx);
    void clear();
    void open(const std::string& path);
    void open(int idx);

    friend std::ostream& operator<<(std::ostream& os, const Mount& mount)
    {
        for(const auto& [path,node]:mount.mount_table)
        {
            os << *node;
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Mount& mount)
    {
        MountNode node;
        while(is >> node)
        {
            std::ifstream ifs(node.path);
            if(!ifs)
            {
                continue;
            }
            mount.mount_table[node.path] = std::make_shared<MountNode>(node);
        }
        for(const auto& [path,node]:mount.mount_table)
        {
            mount.mount_list.push_back(node);
        }
        return is;
    }
};
#define UNICODE
#endif
#endif

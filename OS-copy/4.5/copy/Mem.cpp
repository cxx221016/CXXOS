#include "../include/Mem.h"

Mem::Mem(unsigned int capcity, const std::string &fits) : capcity(capcity), fits(fits)
{
    root = std::make_shared<Dirs>("/CXX", "CXX", nullptr);
    dirMap["/CXX"] = root;
    // root=new Dirs("/CXX","CXX",nullptr);
    current = root;
}

// Mem
int Mem::alloc(int len)
{
    switch (fits[0])
    {
    case 'f':
        return firstfit(len);
    case 'b':
        return bestfit(len);
    case 'w':
        return worstfit(len);
    default:
        std::cerr << "Error: Invalid fit type.\n";
        return -1;
    }
}
void Mem::free(int address, int len)
{
    for (auto it = implMem.begin(); it != implMem.end(); it++)
    {
        if (it->first == address)
        {
            implMem.erase(it);
            return;
        }
    }
}
void Mem::compact()
{
    int curidx = 0;
    for (auto it = implMem.begin(); it != implMem.end(); it++)
    {
        auto file = fileMap[std::make_pair(it->first, it->second)];
        fileMap.erase(std::make_pair(it->first, it->second));
        int len = it->second - it->first + 1;
        it->first = curidx;
        it->second = curidx + len - 1;
        file->address = curidx;
        file->len = len;
        fileMap.insert(std::make_pair(std::make_pair(curidx, curidx + len - 1), file));
        curidx += len;
    }
}

void Mem::status()
{
    for (auto pr : implMem)
    {
        std::cout << pr.first << '\t' << pr.second << "  "<<(*fileMap[pr])<<std::endl;
    }
}

int Mem::firstfit(int len)
{
    if (implMem.empty())
    {
        implMem.push_back(std::make_pair(0, len - 1));
        return 0;
    }
    if (implMem[0].first >= len)
    {
        implMem.insert(implMem.begin(), std::make_pair(0, len - 1));
        return 0;
    }
    for (int i = 1; i < implMem.size(); ++i)
    {
        if (implMem[i].first - implMem[i - 1].second - 1 >= len)
        {
            implMem.insert(implMem.begin() + i, std::make_pair(implMem[i - 1].second + 1, implMem[i - 1].second + len));
            return implMem[i].first;
        }
    }
    if (capcity - implMem[implMem.size() - 1].second - 1 >= len)
    {
        implMem.push_back(std::make_pair(implMem[implMem.size() - 1].second + 1, implMem[implMem.size() - 1].second + len));
        return implMem[implMem.size() - 1].first;
    }
    std::cerr << "Error: No enough space.\n";
    return -1;
}
int Mem::bestfit(int len)
{
    if (implMem.empty())
    {
        implMem.push_back(std::make_pair(0, len - 1));
        return 0;
    }
    int lastsize = 0x3f3f3f3f;
    int lastidx = -1;
    int index = -1;
    if (implMem[0].first >= len)
    {
        lastsize = implMem[0].first;
        lastidx = 0;
        index = 0;
    }
    for (int i = 1; i < implMem.size(); ++i)
    {
        if (implMem[i].first - implMem[i - 1].second - 1 >= len)
        {
            if (implMem[i].first - implMem[i - 1].second - 1 < lastsize)
            {
                lastsize = implMem[i].first - implMem[i - 1].second - 1;
                lastidx = implMem[i - 1].second + 1;
                index = i;
            }
        }
    }
    if (capcity - implMem[implMem.size() - 1].second - 1 >= len)
    {
        if (capcity - implMem[implMem.size() - 1].second - 1 < lastsize)
        {
            lastsize = capcity - implMem[implMem.size() - 1].second - 1;
            lastidx = implMem[implMem.size() - 1].second + 1;
            index = implMem.size();
        }
    }
    if (lastidx == -1)
    {
        std::cerr << "Error: No enough space.\n";
        return -1;
    }
    implMem.insert(implMem.begin() + index, std::make_pair(lastidx, lastidx + len - 1));
    return 0;
}
int Mem::worstfit(int len)
{
    if (implMem.empty())
    {
        implMem.push_back(std::make_pair(0, len - 1));
        return 0;
    }
    int lastsize = -1;
    int lastidx = -1;
    int index = -1;
    if (implMem[0].first >= len)
    {
        lastsize = implMem[0].first;
        lastidx = 0;
        index = 0;
    }
    for (int i = 1; i < implMem.size(); ++i)
    {
        if (implMem[i].first - implMem[i - 1].second - 1 >= len)
        {
            if (implMem[i].first - implMem[i - 1].second - 1 > lastsize)
            {
                lastsize = implMem[i].first - implMem[i - 1].second - 1;
                lastidx = implMem[i - 1].second + 1;
                index = i;
            }
        }
    }
    if (capcity - implMem[implMem.size() - 1].second - 1 >= len)
    {
        if (capcity - implMem[implMem.size() - 1].second - 1 > lastsize)
        {
            lastsize = capcity - implMem[implMem.size() - 1].second - 1;
            lastidx = implMem[implMem.size() - 1].second + 1;
            index = implMem.size();
        }
    }
    if (lastidx == -1)
    {
        std::cerr << "Error: No enough space.\n";
        return -1;
    }
    implMem.insert(implMem.begin() + index, std::make_pair(lastidx, lastidx + len - 1));
    return 0;
}

std::vector<std::shared_ptr<File>> Mem::SearchFile(const std::string &name)
{
    std::vector<std::shared_ptr<File>> res;
    for (auto &file : fileMap)
    {
        if (file.second->name == name)
        {
            res.push_back(file.second);
        }
    }
    return res;
}
std::vector<std::shared_ptr<Dirs>> Mem::SearchDir(const std::string &name)
{
    std::vector<std::shared_ptr<Dirs>> res;
    for (auto &dir : dirMap)
    {
        if (dir.second->name == name)
        {
            res.push_back(dir.second);
        }
    }
    return res;
}

std::shared_ptr<File> Mem::findFile(const std::vector<std::string>& args)
{
    if(args.size()==1)
    {
        return findFile(args[0]);
    }
    else if(args.size()==2)
    {
        return findFile(args[0],args[1]);
    }
    else
    {
        std::cerr<<"Error: Invalid command.\n";
        return nullptr;
    }
}


std::shared_ptr<File> Mem::findFile(const std::string& name)
{
    for(auto& file:current->files)
    {
        if(file->name==name)
        {
            return file;
        }
    }
    std::cerr<<"Error: No such file.\n";
    return nullptr;
}

std::shared_ptr<File> Mem::findFile(const std::string& path,const std::string& name)
{
    auto dir=findDir(path);
    if(dir==nullptr)
    {
        return nullptr;
    }
    for(auto &file:dir->files)
    {
        if(file->name==name)
        {
            return file;
        }
    }
    std::cerr<<"Error: No such file.\n";
    return nullptr;
}

std::shared_ptr<Dirs> Mem::findDir(const std::vector<std::string>& args)
{
    if(args.size()==1)
    {
        return findDir(args[0]);
    }
    else if(args.size()==2)
    {
        return findDir(args[0],args[1]);
    }
    else
    {
        std::cerr<<"Error: Invalid command.\n";
        return nullptr;
    }
}

std::shared_ptr<Dirs> Mem::findDir(const std::string& path,const std::string& name)
{
    auto dir=findDir(path);
    if(dir==nullptr)
    {
        return nullptr;
    }
    for(auto &dir:dir->dirs)
    {
        if(dir->name==name)
        {
            return dir;
        }
    }
    std::cerr<<"Error: No such directory.\n";
    return nullptr;
}


std::shared_ptr<Dirs> Mem::findDir(const std::string& path)
{
    for(auto& dir:current->dirs)
    {
        if(dir->name==path)
        {
            return dir;
        }
    }
    if(dirMap.find(path)==dirMap.end())
    {
        std::cerr<<"Error: No such directory.\n";
        return nullptr;
    }
    return dirMap[path];
}

void Mem::addFile(const std::string& path,const File& file)
{
    addFile(path,file.name,file.len,file.data);
}

void Mem::addFile(const std::string& path,std::shared_ptr<File> file)
{
    addFile(path,file->name,file->len,file->data);
}

void Mem::addFile(const std::string& path,const std::string& name,int len,const std::string& data)
{
    auto clonecur=current;
    auto dir=findDir(path);
    if(dir==nullptr)
    {
        return;
    }
    current=dir;
    type(name,len,data);
    current=clonecur;
}

void Mem::addDir(const std::string& path,const Dirs& dir)
{
    addDir(path,dir.name);
}

void Mem::addDir(const std::string& path,std::shared_ptr<Dirs> dir)
{
    addDir(path,dir->name);
}

void Mem::addDir(const std::string& path,const std::string& name)
{
    auto clonecur=current;
    auto dir=findDir(path);
    if(dir==nullptr)
    {
        return;
    }
    current=dir;
    md(name);
    current=clonecur;

}

void Mem::eraseFile(std::shared_ptr<File> file)
{
    fileMap.erase(std::make_pair(file->address, file->address + file->len - 1));
    free(file->address, file->len);
}

void Mem::eraseDir(std::shared_ptr<Dirs> dir)
{
    for (auto &file : dir->files)
    {
        eraseFile(file);
    }
    dir->files.clear();
    for (auto &subdir : dir->dirs)
    {
        eraseDir(subdir);
    }
    dirMap.erase(dir->path);
}

void Mem::md(const std::string &name) // make directory
{
    if (name.find('/') != std::string::npos)
    {
        std::cerr << "Error: Invalid directory name.\n";
        return;
    }
    for (auto &dir : current->dirs)
    {
        if (dir->name == name)
        {
            std::cerr << "Error: Directory already exists.\n";
            return;
        }
    }
    std::string path = current->path + "/" + name;
    auto newDir = std::make_shared<Dirs>(path, name, current);
    current->dirs.push_back(newDir);
    dirMap[path] = current->dirs.back();
    std::cout << "Directory created.\n";
}
void Mem::rd(const std::string &name) // remove directory
{
    if (name == "*" || name == "." || name == "..")
    {
        for(auto & dir : current->dirs)
        {
            eraseDir(dir);
        }
        current->dirs.clear();
        return;
    }
    if (name.find('/') != std::string::npos)
    {
        std::cerr << "Error: Invalid directory name.\n";
        return;
    }
    for (auto it = current->dirs.begin(); it != current->dirs.end(); )
    {
        if ((*it)->name == name)
        {
            //dirMap.erase((*it)->path);
            eraseDir(*it);
            it=current->dirs.erase(it);
            std::cout << "Directory removed.\n";
            return;
        }
        else it++;
    }
    std::cerr << "Error: Directory not found.\n";
}
void Mem::type(const std::string &name, int len, const std::string &data) // type file
{
    if (name.find('/') != std::string::npos)
    {
        std::cerr << "Error: Invalid file name.\n";
        return;
    }
    for (auto &file : current->files)
    {
        //std::cout<<file->name<<std::endl;
        if (file->name == name)
        {
            std::cout << "Error: file already exists.\n"
                      << std::endl;
            return;
        }
    }
    int address = alloc(len);
    if (address == -1)
    {
        // std::cerr<<"Error: No enough space.\n";
        return;
    }
    std::string path = current->path;
    auto newFile = std::make_shared<File>(path, name, address, len, data);
    fileMap[std::make_pair(address, address + len - 1)] = newFile;
    // File* newFile=new File(path,name,address,len,data);
    current->files.push_back(newFile);
}
void Mem::del(const std::string &name) // delete file
{
    if (name == "*" || name == "." || name == "..")
    {
        for (auto it = current->files.begin(); it != current->files.end();)
        {
            fileMap.erase(std::make_pair((*it)->address, (*it)->address + (*it)->len - 1));
            free((*it)->address, (*it)->len);
            it=current->files.erase(it);
        }
        return;
    }
    if (name.find('/') != std::string::npos)
    {
        std::cerr << "Error: Invalid file name.\n";
        return;
    }
    for (auto it = current->files.begin(); it != current->files.end(); )
    {
        if ((*it)->name == name)
        {
            fileMap.erase(std::make_pair((*it)->address, (*it)->address + (*it)->len - 1));
            free((*it)->address, (*it)->len);
            it=current->files.erase(it);
            std::cout << "File deleted.\n";
            return;
        }
        else it++;
    }
    std::cerr << "Error: File not found.\n";
}

void Mem::ren(const std::vector<std::string>& args,std::string newname)
{
    auto file=findFile(args);
    auto dir=findDir(args);
    if(file!=nullptr && dir!=nullptr)
    {
        std::cerr<<"Error: File and directory have the same name.\n";
        return;
    }
    if(file==nullptr && dir==nullptr)
    {
        std::cerr<<"Error: File or directory not found.\n";
        return;
    }
    if(file!=nullptr)
    {
        file->name=newname;
    }
    if(dir!=nullptr)
    {
        dir->name=newname;
    }
}

void Mem::edit(const std::vector<std::string>& args)
{
    auto file=findFile(args);
    if(file==nullptr)
    {
        std::cerr<<"Error: File not found.\n";
        return;
    }
    std::cout<<"begin to edit file "<<file->name<<std::endl;
    std::string newdata;
    std::cin>>newdata;
    file->data=newdata;
    std::cout<<"edit finished.\n";
}

void Mem::fc(const std::vector<std::string>& args1,const std::vector<std::string>& args2)
{
    auto file1=findFile(args1);
    auto file2=findFile(args2);
    if(file1==nullptr || file2==nullptr)
    {
        std::cerr<<"Error: File not found.\n";
        return;
    }
    if(file1->data==file2->data)
    {
        std::cout<<"Files are the same.\n";
    }
    else
    {
        std::cout<<"Files are different.\n";
        //std::cout<<"file1data: "<<file1->data<<"  file2data: "<<file2->data<<std::endl;
        printf("file1data: %-20s  file2data: %-20s\n",file1->data.c_str(),file2->data.c_str());
        std::cout<<"Different part:\n";
        int i=0;
        for(;i<file1->data.size() && i<file2->data.size();i++)
        {
            if(file1->data[i]!=file2->data[i])
            {
                //std::cout<<"idx: "<<i<<"file1data: "<<file1->data[i]<<"  file2data: "<<file2->data[i]<<std::endl;
                printf("idx: %-5d  file1data: %-20c  file2data: %-20c\n",i,file1->data[i],file2->data[i]);
            }
        }
        while(i<file1->data.size())
        {
            //std::cout<<"idx: "<<i<<"file1data: "<<file1->data[i]<<"  file2data: null"<<std::endl;
            printf("idx: %-5d  file1data: %-20c  file2data: %-20s\n",i,file1->data[i],"null");
            ++i;
        }
        while(i<file2->data.size())
        {
            //std::cout<<"idx: "<<i<<"file1data: null"<<"  file2data: "<<file2->data[i]<<std::endl;
            printf("idx: %-5d  file1data: %-20s  file2data: %-20c\n",i,"null",file2->data[i]);
            ++i;
        }

    }
}
void Mem::cat(const std::vector<std::string>& args)
{
    auto file=findFile(args);
    if(file==nullptr)
    {
        std::cerr<<"Error: File not found.\n";
        return;
    }
    std::cout<<file->data<<std::endl;
}

void Mem::chmod(const std::string& path,std::vector<std::string>& args)
{
    auto dir=findDir(path);
    if(dir==nullptr)
    {
        return;
    }
    implchmod(dir,args);
}

void Mem::chmod(const std::string& path,const std::string& name,std::vector<std::string>& args)
{
    auto file=findFile(path,name);
    if(file==nullptr)
    {
        return;
    }
    implchmod(file,args);
}

template<typename T>
void Mem::implchmod(T t,std::vector<std::string>& args)
{
    static std::unordered_map<char,int> filelimits
    {
        {'r',1},
        {'w',2},
        {'x',3}
    };
    static std::unordered_map<char,int> dirlimits
    {
        {'r',1},
        {'w',2}
    };

    std::shared_ptr<std::unordered_map<char,int>> tmp;
    if(std::is_same_v<T,std::shared_ptr<File>>)
    {
        tmp=std::make_shared<std::unordered_map<char,int>>(filelimits);
    }
    else if(std::is_same_v<T,std::shared_ptr<Dirs>>)
    {
        tmp=std::make_shared<std::unordered_map<char,int>>(dirlimits);
    }

    for(auto & arg:args)
    {
        int op=arg[0];
        if(op=='+')
        {
            for(int i=1;i<arg.size();++i)
            {
                if(tmp->count(arg[i]))
                {
                    t->limits[(*tmp)[arg[i]]]=arg[i];
                }
            }
        }
        else if(op=='-')
        {
            for(int i=1;i<arg.size();++i)
            {
                if(tmp->count(arg[i]))
                {
                    t->limits[(*tmp)[arg[i]]]='-';
                }
            }
        }
        else
        {
            std::cerr<<"Error: Invalid argument.\n";
            return;
        }
    }
}

void Mem::showmod(const std::string& path,const std::string& name)
{
    auto file=findFile(path,name);
    if(file==nullptr)
    {
        return;
    }
    std::cout<<file->limits<<std::endl;
}

void Mem::showmod(const std::string& path)
{
    auto dir=findDir(path);
    if(dir==nullptr)
    {
        return;
    }
    std::cout<<dir->limits<<std::endl;
}

void Mem::cd(const std::string &name) // change directory
{
    if (name.find('/') != std::string::npos)
    {
        std::string first = name.substr(0, 2);
        if (first == "..")
        {
            std::string second = name.substr(3);
            if (!current->pre)
            {
                std::cerr << "Error: Already in root directory.\n";
                return;
            }
            current = current->pre;
            cd(second);
        }
        else if (first == "./")
        {
            std::string second = name.substr(2);
            cd(second);
        }
        else
        {
            std::string second = name.substr(0, name.find('/'));
            for (auto &dir : current->dirs)
            {
                if (dir->name == second)
                {
                    current = dir;
                    cd(name.substr(name.find('/') + 1));
                    return;
                }
            }
            std::cerr << "Error: Directory not found.\n";
        }
        return;
    }
    if (name == "..")
    {
        if (!current->pre)
        {
            std::cerr << "Error: Already in root directory.\n";
            return;
        }
        std::string path = current->path;
        path = path.substr(0, path.find_last_of('/'));
        // current=dirMap[path];
        current = current->pre;
        return;
    }
    for (auto &dir : current->dirs)
    {
        if (dir->name == name)
        {
            current = dir;
            return;
        }
    }
    std::cerr << "Error: Directory not found.\n";
}

void Mem::copy(const std::string& src,const std::string& dst)
{
    std::string path=src.substr(0,src.find_last_of('/'));
    std::string name=src.substr(src.find_last_of('/')+1);
    if(path==src)
    {
        path=current->path;
    }
    //std::cout<<path<<'\t'<<name<<'\t'<<dst<<'\t'<<std::endl;
    auto file =findFile(path,name);
    if(!file)
    {
        std::cerr<<"Error: File not found.\n";
        return;
    }
    auto dir=findDir(dst);
    if(!dir)
    {
        std::cerr<<"Error: Directory not found.\n";
        return;
    }
    int address=alloc(file->len);
    if(address==-1)
    {
        return;
    }
    auto newFile=std::make_shared<File>(dir->path,file->name,address,file->len,file->data);
    fileMap[std::make_pair(address,address+file->len-1)]=newFile;
    dir->files.push_back(newFile);
}

void Mem::dir() // list directory
{
    for (auto &dir : current->dirs)
    {
        // std::cout<<dir.name<<'\t';
        //printf("name: %-20s path: %-10s limits: %s\n", dir->name.c_str(), dir->path.c_str(),dir->limits.c_str());
        std::cout<<*dir;
    }
    for (auto &file : current->files)
    {
        // std::cout<<file.name<<'\t';
        //printf("name: %-20s path: %-10s address: %-10d len: %-10d data: %s limits: %s\n", file->name.c_str(), file->path.c_str(), file->address, file->len,file->data.c_str(),file->limits.c_str());
        std::cout<<*file;
    }
}

std::string Mem::getcurdir()
{
    return current->path;
}

void Mem::debug()
{
    auto addt = [](int level) -> std::string
    {
        std::string s;
        for (int i = 0; i < level; i++)
        {
            s += "\t";
        }
        return s;
    };
    std::stack<std::pair<std::shared_ptr<Dirs>, int>> q;
    q.emplace(root, 0);
    while (!q.empty())
    {
        auto cur = q.top();
        q.pop();
        std::cout << addt(cur.second) << cur.first->path << std::endl;
        /*
        for(auto & dir:cur.first->dirs)
        {
            q.emplace(dir,cur.second+1);
        }
        for(auto& file:cur.first->files)
        {
            std::cout<<file->name<<'\t'<<file->data<<std::endl;
        }*/
        for (auto it = cur.first->dirs.rbegin(); it != cur.first->dirs.rend(); it++)
        {
            q.emplace(*it, cur.second + 1);
        }
        for (auto it = cur.first->files.rbegin(); it != cur.first->files.rend(); it++)
        {
            std::cout << addt(cur.second + 1) << (*it)->name << '\t' << (*it)->data << std::endl;
        }
    }
}

void Mem::toSerial(const std::string& path,const Mem& mem)
{
    std::ofstream ofs(path);
    if(!ofs.is_open())
    {
        std::cout<<"open file failed\n";
        return;
    }
    ofs<<mem;
    //std::cout<<"file path: "<<path<<std::endl;
}

bool Mem::deSerial(const std::string& filepath,Mem& mem)
{
    std::ifstream ifs(filepath);
    if(!ifs.is_open())
    {
        std::cout<<"open file failed\n";
        return false;
    }
    std::string line;
    std::string type;
    std::string path;
    std::string name;
    int address;
    int len;
    std::string data;
    std::stringstream ss;
    std::string pass;

    //capcity
    getline(ifs,line);
    ss.clear();
    ss.str(line);
    ss>>pass>>mem.capcity;

    //fits
    getline(ifs,line);
    ss.clear();
    ss.str(line);
    ss>>pass>>mem.fits;

    auto root=std::make_shared<Dirs>("/CXX","CXX",nullptr);
    mem.dirMap["/CXX"]=root;
    auto cur=root;
    std::queue<std::shared_ptr<Dirs>> q;
    bool flag=true;
    while((!q.empty()||flag)&&getline(ifs,line))
    {
        flag=false;
        if(line=="---")
        {
            cur=q.front();
            q.pop();
            continue;
        }
        std::stringstream ss(line);
        ss>>type;
        if(type=="file")
        {
            ss>>path>>name>>address>>len>>data;
            auto file=std::make_shared<File>(path,name,address,len,data);
            cur->files.push_back(file);
            mem.fileMap[std::make_pair(address,address+len-1)]=file;
            flag=true;
        }
        else if(type=="Dirs")
        {
            ss>>path>>name;
            auto dir=std::make_shared<Dirs>(path,name,cur);
            cur->dirs.push_back(dir);
            mem.dirMap[path]=dir;
            q.push(dir);
        }
    }
    mem.root=root;
    std::cout<<"file system derial success\n";

    getline(ifs,line);
    //implMem
    getline(ifs,line);
    ss.clear();
    ss.str(line);
    ss>>pass;
    int first,second;
    while(ss>>first>>second)
    {
        mem.implMem.push_back(std::make_pair(first,second));
    }
    std::cout<<"implMem derial success\n";

    //dirMap
    getline(ifs,line);
    /*
    ss.clear();
    ss.str(line);
    ss>>pass;
    while(ss>>path)
    {
        mem.cd(path);
        mem.dirMap[path]=mem.current;
    }
    std::cout<<"dirMap derial success\n";
    */


    //fileMap
    getline(ifs,line);
    /*
    ss.clear();
    ss.str(line);
    ss>>pass;
    while(ss>>first>>second>>path>>name)
    {
        mem.fileMap[std::make_pair(first,second)]=mem.findFile(path,name);
    }
    */
    std::cout<<"fileMap derial success\n";
    mem.current=root;

    return true;
}

// impl Mem
unsigned int Mem::getcapcity()
{
    return capcity;
}

std::ostream& operator<<(std::ostream& os,const Mem& mem)
{
    os<<"capcity"<<' '<<mem.capcity<<'\n';
    os<<"fits"<<' '<<mem.fits<<'\n';
    std::queue<std::shared_ptr<Dirs>> q;
    q.push(mem.root);
    while(!q.empty())
    {
        auto cur=q.front();
        q.pop();
        //os<<cur->path<<std::endl;
        for(auto& file:cur->files)
        {
            //os<<"file"<<' '<<file->path<<' '<<file->name<<' '<<file->address<<' '<<file->len<<' '<<file->data<<' '<<file->limits<<'\n';
            os<<"file"<<' '<<*file;
        }
        for(auto& dir:cur->dirs)
        {
            //os<<"Dirs"<<' '<<dir->path<<' '<<dir->name<<' '<<dir->limits<<'\n';
            os<<"Dirs"<<' '<<*dir;
            q.push(dir);
        }
        os<<"---"<<'\n';

    }
    os<<"implMem"<<' ';
    for(auto& i:mem.implMem)
    {
        os<<i.first<<' '<<i.second<<' ';
    }
    os<<'\n';

    os<<"dirMap"<<' ';
    for(auto& i:mem.dirMap)
    {
        os<<i.first<<' ';
    }
    os<<'\n';

    os<<"fileMap"<<' ';
    for(auto& i:mem.fileMap)
    {
        os<<i.first.first<<' '<<i.first.second<<' '<<i.second->path<<' '<<i.second->name<<' ';
    }
    os<<'\n';
    return os;
}

std::istream& operator>>(std::istream& ifs,Mem& mem)
{
    std::string line;
    std::string type;
    std::string path;
    std::string name;
    std::string limits;
    int address;
    int len;
    std::string data;
    std::stringstream ss;
    std::string pass;

    //capcity
    getline(ifs,line);
    ss.clear();
    ss.str(line);
    ss>>pass>>mem.capcity;

    //fits
    getline(ifs,line);
    ss.clear();
    ss.str(line);
    ss>>pass>>mem.fits;

    auto root=std::make_shared<Dirs>("/CXX","CXX",nullptr);
    mem.dirMap["/CXX"]=root;
    auto cur=root;
    std::queue<std::shared_ptr<Dirs>> q;
    bool flag=true;
    while((!q.empty()||flag)&&getline(ifs,line))
    {
        flag=false;
        if(line=="---")
        {
            cur=q.front();
            q.pop();
            continue;
        }
        std::stringstream ss(line);
        ss>>type;
        if(type=="file")
        {
            //ss>>path>>name>>address>>len>>data>>limits;
            //auto file=std::make_shared<File>(path,name,address,len,data,limits);
            File tmpfile;
            ss>>tmpfile;
            auto file=std::make_shared<File>(tmpfile);
            cur->files.push_back(file);
            mem.fileMap[std::make_pair(file->address,file->address+len-1)]=file;
            flag=true;
        }
        else if(type=="Dirs")
        {
            //ss>>path>>name>>limits;
            //auto dir=std::make_shared<Dirs>(path,name,cur,limits);
            Dirs tmpdir;
            ss>>tmpdir;
            tmpdir.pre=cur;
            auto dir=std::make_shared<Dirs>(tmpdir);
            cur->dirs.push_back(dir);
            mem.dirMap[dir->path]=dir;
            q.push(dir);
        }
    }
    mem.root=root;
    std::cout<<"file system derial success\n";

    getline(ifs,line);
    //implMem
    getline(ifs,line);
    ss.clear();
    ss.str(line);
    ss>>pass;
    int first,second;
    while(ss>>first>>second)
    {
        mem.implMem.push_back(std::make_pair(first,second));
    }
    std::cout<<"implMem derial success\n";

    //dirMap
    getline(ifs,line);
    /*
    ss.clear();
    ss.str(line);
    ss>>pass;
    while(ss>>path)
    {
        mem.cd(path);
        mem.dirMap[path]=mem.current;
    }
    std::cout<<"dirMap derial success\n";
    */


    //fileMap
    getline(ifs,line);
    /*
    ss.clear();
    ss.str(line);
    ss>>pass;
    while(ss>>first>>second>>path>>name)
    {
        mem.fileMap[std::make_pair(first,second)]=mem.findFile(path,name);
    }
    */
    std::cout<<"fileMap derial success\n";
    mem.current=root;
    return ifs;
}
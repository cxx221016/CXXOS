#include "../include/BinarySerial.h"
std::size_t BinarySerial::getsize(const std::string &filename)
{
    std::ifstream fin(filename, std::ios::binary);
    fin.seekg(0, std::ios::end);
    std::size_t size = fin.tellg();
    fin.close();
    return size;
}

void BinarySerial::cxxsha(char& c)
{
    c=c^0x5a;
}

void BinarySerial::cxxsha(char* ch, std::size_t size)
{
    for(int i = 0; i < size; ++i)
    {
        cxxsha(ch[i]);
    }
}

 std::string BinarySerial::deSerial(const std::string& filename)
{
    std::ifstream fin(filename);
    if(!fin.is_open())
    {
        return std::string();
    }
    std::stringstream ss;
    ss << fin.rdbuf();
    fin.close();
    return ss.str();
}

void BinarySerial::BinarytoSerial(const std::string &filename, const std::string &serialdata)
{
    //std::size_t size = getsize(filename);
    std::size_t size = serialdata.size();
    char *data = new char[size];
    for(int i = 0; i < size; ++i)
    {
        data[i] = serialdata[i];
    }
    cxxsha(data, size);
    int blocknum = size / blocksize;
    int lastblocksize = size % blocksize;
    std::ofstream fout(filename, std::ios::binary);
    if(!fout.is_open())
    {
        std::cout<<filename<<" ";
        std::cout << "open file failed" << std::endl;
        return;
    }
    while(blocknum--)
    {
        fout.write(data, blocksize);
        data += blocksize;
    }
    if(lastblocksize)
    {
        fout.write(data, lastblocksize);
    }
    fout.close();
}
std::string BinarySerial::BinarydeSerial(const std::string &filename)
{
    std::ifstream test(filename, std::ios::binary);
    if(!test.is_open())
    {
        return std::string();
    }
    test.close();
    std::size_t size = getsize(filename);
    char *buf=new char [blocksize];
    int blocknum = size / blocksize;
    int lastblocksize = size % blocksize;
    std::string serialdata;
    std::ifstream fin(filename, std::ios::binary);
    while(blocknum--)
    {
        fin.read(buf, blocksize);
        cxxsha(buf, blocksize);
        serialdata.append(buf, blocksize);
    }
    if(lastblocksize)
    {
        fin.read(buf, lastblocksize);
        cxxsha(buf, lastblocksize);
        serialdata.append(buf, lastblocksize);
    }
    fin.close();
    return serialdata;
}

std::string BinarySerial::ChooseSerial(const std::string& filename)
{
    auto idx=filename.find_last_of('.');
    auto suffix=filename.substr(idx+1);
    if(suffix=="bin") return BinarydeSerial(filename);
    else return deSerial(filename);
    return std::string();
}

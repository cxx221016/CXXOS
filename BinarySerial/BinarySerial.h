#pragma once
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>

class BinarySerial
{
    constexpr static std::size_t blocksize = 4;
public:
    static void cxxsha(char& c);
    static void cxxsha(char* ch, std::size_t size);
    static std::size_t getsize(const std::string& filename);
    static std::string getstringdata(const std::string& filename);
    static void BinarytoSerial(const std::string& filename, const std::string &serialdata);
    static std::string BinarydeSerial(const std::string& filename);
};
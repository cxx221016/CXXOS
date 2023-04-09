#pragma
#include<unordered_map>
#include<functional>
#include<string>
#include<iostream>

using namespace std::string_literals;

std::unordered_map<std::string, std::function<int(int,int)>> opmap
{
    {"add"s, [](int a,int b){ return a+b;}},
    {"sub"s, [](int a,int b){ return a-b;}},
    {"mul"s, [](int a,int b){ return a*b;}},
    {"div"s, [](int a,int b){ return a/b;}},
    {"mod"s, [](int a,int b){ return a%b;}},
    {"and"s, [](int a,int b){ return a&b;}},
    {"or"s, [](int a,int b){ return a|b;}},
    {"xor"s, [](int a,int b){ return a^b;}},
    {"not"s, [](int a,int b){ return ~a;}},
    {"shl"s, [](int a,int b){ return a<<b;}},
    {"shr"s, [](int a,int b){ return a>>b;}},
    {"eq"s, [](int a,int b){ return a==b;}},
    {"neq"s, [](int a,int b){ return a!=b;}},
    {"less"s, [](int a,int b){ return a<b;}},
    {"leq"s, [](int a,int b){ return a<=b;}},
    {"great"s, [](int a,int b){ return a>b;}},
    {"geq"s, [](int a,int b){ return a>=b;}},
    {"hello"s,[](int a,int b){std::cout<<"Hello World!\n";return 0;}}
};
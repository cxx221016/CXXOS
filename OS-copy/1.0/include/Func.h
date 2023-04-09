#pragma
#include<unordered_map>
#include<functional>
#include<string>

using namespace std::string_literals;

std::unordered_map<std::string, std::function<int(int,int)>> opmap
{
    {"add"s, [](int a,int b){ return a+b;}},
    {"sub"s, [](int a,int b){ return a-b;}},
    {"mul"s, [](int a,int b){ return a*b;}},
    {"div"s, [](int a,int b){ return a/b;}}
};
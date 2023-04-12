@echo off
cd ..
rd /S/Q build
md build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make   
cd ../bin
main.exe

cmake_minimum_required(VERSION 3.10)
project(cxxos)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(Mem STATIC ${PROJECT_SOURCE_DIR}/src/Mem.cpp)
add_library(VMem STATIC ${PROJECT_SOURCE_DIR}/src/VMem.cpp)

add_executable(main ${PROJECT_SOURCE_DIR}/main.cpp)
target_link_libraries(main Mem VMem)
# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.26

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "D:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\MY_CODE\CXXOS\OS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\MY_CODE\CXXOS\OS\build

# Include any dependencies generated for this target.
include CMakeFiles/Mem.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Mem.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Mem.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Mem.dir/flags.make

CMakeFiles/Mem.dir/src/Mem.cpp.obj: CMakeFiles/Mem.dir/flags.make
CMakeFiles/Mem.dir/src/Mem.cpp.obj: CMakeFiles/Mem.dir/includes_CXX.rsp
CMakeFiles/Mem.dir/src/Mem.cpp.obj: D:/MY_CODE/CXXOS/OS/src/Mem.cpp
CMakeFiles/Mem.dir/src/Mem.cpp.obj: CMakeFiles/Mem.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\MY_CODE\CXXOS\OS\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Mem.dir/src/Mem.cpp.obj"
	D:\CodeBlocks\MinGW\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Mem.dir/src/Mem.cpp.obj -MF CMakeFiles\Mem.dir\src\Mem.cpp.obj.d -o CMakeFiles\Mem.dir\src\Mem.cpp.obj -c D:\MY_CODE\CXXOS\OS\src\Mem.cpp

CMakeFiles/Mem.dir/src/Mem.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Mem.dir/src/Mem.cpp.i"
	D:\CodeBlocks\MinGW\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\MY_CODE\CXXOS\OS\src\Mem.cpp > CMakeFiles\Mem.dir\src\Mem.cpp.i

CMakeFiles/Mem.dir/src/Mem.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Mem.dir/src/Mem.cpp.s"
	D:\CodeBlocks\MinGW\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\MY_CODE\CXXOS\OS\src\Mem.cpp -o CMakeFiles\Mem.dir\src\Mem.cpp.s

# Object files for target Mem
Mem_OBJECTS = \
"CMakeFiles/Mem.dir/src/Mem.cpp.obj"

# External object files for target Mem
Mem_EXTERNAL_OBJECTS =

D:/MY_CODE/CXXOS/OS/lib/libMem.a: CMakeFiles/Mem.dir/src/Mem.cpp.obj
D:/MY_CODE/CXXOS/OS/lib/libMem.a: CMakeFiles/Mem.dir/build.make
D:/MY_CODE/CXXOS/OS/lib/libMem.a: CMakeFiles/Mem.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\MY_CODE\CXXOS\OS\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library D:\MY_CODE\CXXOS\OS\lib\libMem.a"
	$(CMAKE_COMMAND) -P CMakeFiles\Mem.dir\cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\Mem.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Mem.dir/build: D:/MY_CODE/CXXOS/OS/lib/libMem.a
.PHONY : CMakeFiles/Mem.dir/build

CMakeFiles/Mem.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\Mem.dir\cmake_clean.cmake
.PHONY : CMakeFiles/Mem.dir/clean

CMakeFiles/Mem.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\MY_CODE\CXXOS\OS D:\MY_CODE\CXXOS\OS D:\MY_CODE\CXXOS\OS\build D:\MY_CODE\CXXOS\OS\build D:\MY_CODE\CXXOS\OS\build\CMakeFiles\Mem.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Mem.dir/depend


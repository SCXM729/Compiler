# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sheep/Compiler/cx

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sheep/Compiler/cx/build

# Include any dependencies generated for this target.
include toolcpp/CMakeFiles/compiler.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include toolcpp/CMakeFiles/compiler.dir/compiler_depend.make

# Include the progress variables for this target.
include toolcpp/CMakeFiles/compiler.dir/progress.make

# Include the compile flags for this target's objects.
include toolcpp/CMakeFiles/compiler.dir/flags.make

toolcpp/CMakeFiles/compiler.dir/compiler.cpp.o: toolcpp/CMakeFiles/compiler.dir/flags.make
toolcpp/CMakeFiles/compiler.dir/compiler.cpp.o: /home/sheep/Compiler/cx/toolcpp/compiler.cpp
toolcpp/CMakeFiles/compiler.dir/compiler.cpp.o: toolcpp/CMakeFiles/compiler.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/sheep/Compiler/cx/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object toolcpp/CMakeFiles/compiler.dir/compiler.cpp.o"
	cd /home/sheep/Compiler/cx/build/toolcpp && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT toolcpp/CMakeFiles/compiler.dir/compiler.cpp.o -MF CMakeFiles/compiler.dir/compiler.cpp.o.d -o CMakeFiles/compiler.dir/compiler.cpp.o -c /home/sheep/Compiler/cx/toolcpp/compiler.cpp

toolcpp/CMakeFiles/compiler.dir/compiler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/compiler.dir/compiler.cpp.i"
	cd /home/sheep/Compiler/cx/build/toolcpp && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sheep/Compiler/cx/toolcpp/compiler.cpp > CMakeFiles/compiler.dir/compiler.cpp.i

toolcpp/CMakeFiles/compiler.dir/compiler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/compiler.dir/compiler.cpp.s"
	cd /home/sheep/Compiler/cx/build/toolcpp && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sheep/Compiler/cx/toolcpp/compiler.cpp -o CMakeFiles/compiler.dir/compiler.cpp.s

# Object files for target compiler
compiler_OBJECTS = \
"CMakeFiles/compiler.dir/compiler.cpp.o"

# External object files for target compiler
compiler_EXTERNAL_OBJECTS =

toolcpp/libcompiler.a: toolcpp/CMakeFiles/compiler.dir/compiler.cpp.o
toolcpp/libcompiler.a: toolcpp/CMakeFiles/compiler.dir/build.make
toolcpp/libcompiler.a: toolcpp/CMakeFiles/compiler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/sheep/Compiler/cx/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libcompiler.a"
	cd /home/sheep/Compiler/cx/build/toolcpp && $(CMAKE_COMMAND) -P CMakeFiles/compiler.dir/cmake_clean_target.cmake
	cd /home/sheep/Compiler/cx/build/toolcpp && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/compiler.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
toolcpp/CMakeFiles/compiler.dir/build: toolcpp/libcompiler.a
.PHONY : toolcpp/CMakeFiles/compiler.dir/build

toolcpp/CMakeFiles/compiler.dir/clean:
	cd /home/sheep/Compiler/cx/build/toolcpp && $(CMAKE_COMMAND) -P CMakeFiles/compiler.dir/cmake_clean.cmake
.PHONY : toolcpp/CMakeFiles/compiler.dir/clean

toolcpp/CMakeFiles/compiler.dir/depend:
	cd /home/sheep/Compiler/cx/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sheep/Compiler/cx /home/sheep/Compiler/cx/toolcpp /home/sheep/Compiler/cx/build /home/sheep/Compiler/cx/build/toolcpp /home/sheep/Compiler/cx/build/toolcpp/CMakeFiles/compiler.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : toolcpp/CMakeFiles/compiler.dir/depend

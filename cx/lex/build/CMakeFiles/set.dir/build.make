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
CMAKE_SOURCE_DIR = /home/sheep/Compiler/cx/lex

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sheep/Compiler/cx/lex/build

# Include any dependencies generated for this target.
include CMakeFiles/set.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/set.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/set.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/set.dir/flags.make

CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.o: CMakeFiles/set.dir/flags.make
CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.o: /home/sheep/Compiler/cx/tool/set.cpp
CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.o: CMakeFiles/set.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/sheep/Compiler/cx/lex/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.o -MF CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.o.d -o CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.o -c /home/sheep/Compiler/cx/tool/set.cpp

CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sheep/Compiler/cx/tool/set.cpp > CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.i

CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sheep/Compiler/cx/tool/set.cpp -o CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.s

# Object files for target set
set_OBJECTS = \
"CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.o"

# External object files for target set
set_EXTERNAL_OBJECTS =

libset.a: CMakeFiles/set.dir/home/sheep/Compiler/cx/tool/set.cpp.o
libset.a: CMakeFiles/set.dir/build.make
libset.a: CMakeFiles/set.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/sheep/Compiler/cx/lex/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libset.a"
	$(CMAKE_COMMAND) -P CMakeFiles/set.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/set.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/set.dir/build: libset.a
.PHONY : CMakeFiles/set.dir/build

CMakeFiles/set.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/set.dir/cmake_clean.cmake
.PHONY : CMakeFiles/set.dir/clean

CMakeFiles/set.dir/depend:
	cd /home/sheep/Compiler/cx/lex/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sheep/Compiler/cx/lex /home/sheep/Compiler/cx/lex /home/sheep/Compiler/cx/lex/build /home/sheep/Compiler/cx/lex/build /home/sheep/Compiler/cx/lex/build/CMakeFiles/set.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/set.dir/depend


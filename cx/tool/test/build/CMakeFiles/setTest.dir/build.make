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
CMAKE_SOURCE_DIR = /home/sheep/Compiler/cx/tool/test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sheep/Compiler/cx/tool/test/build

# Include any dependencies generated for this target.
include CMakeFiles/setTest.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/setTest.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/setTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/setTest.dir/flags.make

CMakeFiles/setTest.dir/setTest.cpp.o: CMakeFiles/setTest.dir/flags.make
CMakeFiles/setTest.dir/setTest.cpp.o: /home/sheep/Compiler/cx/tool/test/setTest.cpp
CMakeFiles/setTest.dir/setTest.cpp.o: CMakeFiles/setTest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/sheep/Compiler/cx/tool/test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/setTest.dir/setTest.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/setTest.dir/setTest.cpp.o -MF CMakeFiles/setTest.dir/setTest.cpp.o.d -o CMakeFiles/setTest.dir/setTest.cpp.o -c /home/sheep/Compiler/cx/tool/test/setTest.cpp

CMakeFiles/setTest.dir/setTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/setTest.dir/setTest.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sheep/Compiler/cx/tool/test/setTest.cpp > CMakeFiles/setTest.dir/setTest.cpp.i

CMakeFiles/setTest.dir/setTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/setTest.dir/setTest.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sheep/Compiler/cx/tool/test/setTest.cpp -o CMakeFiles/setTest.dir/setTest.cpp.s

# Object files for target setTest
setTest_OBJECTS = \
"CMakeFiles/setTest.dir/setTest.cpp.o"

# External object files for target setTest
setTest_EXTERNAL_OBJECTS =

setTest: CMakeFiles/setTest.dir/setTest.cpp.o
setTest: CMakeFiles/setTest.dir/build.make
setTest: libset.a
setTest: CMakeFiles/setTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/sheep/Compiler/cx/tool/test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable setTest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/setTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/setTest.dir/build: setTest
.PHONY : CMakeFiles/setTest.dir/build

CMakeFiles/setTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/setTest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/setTest.dir/clean

CMakeFiles/setTest.dir/depend:
	cd /home/sheep/Compiler/cx/tool/test/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sheep/Compiler/cx/tool/test /home/sheep/Compiler/cx/tool/test /home/sheep/Compiler/cx/tool/test/build /home/sheep/Compiler/cx/tool/test/build /home/sheep/Compiler/cx/tool/test/build/CMakeFiles/setTest.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/setTest.dir/depend


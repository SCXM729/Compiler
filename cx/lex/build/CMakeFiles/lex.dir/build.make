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
include CMakeFiles/lex.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/lex.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/lex.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lex.dir/flags.make

CMakeFiles/lex.dir/lex.cpp.o: CMakeFiles/lex.dir/flags.make
CMakeFiles/lex.dir/lex.cpp.o: /home/sheep/Compiler/cx/lex/lex.cpp
CMakeFiles/lex.dir/lex.cpp.o: CMakeFiles/lex.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/sheep/Compiler/cx/lex/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/lex.dir/lex.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lex.dir/lex.cpp.o -MF CMakeFiles/lex.dir/lex.cpp.o.d -o CMakeFiles/lex.dir/lex.cpp.o -c /home/sheep/Compiler/cx/lex/lex.cpp

CMakeFiles/lex.dir/lex.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/lex.dir/lex.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sheep/Compiler/cx/lex/lex.cpp > CMakeFiles/lex.dir/lex.cpp.i

CMakeFiles/lex.dir/lex.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/lex.dir/lex.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sheep/Compiler/cx/lex/lex.cpp -o CMakeFiles/lex.dir/lex.cpp.s

# Object files for target lex
lex_OBJECTS = \
"CMakeFiles/lex.dir/lex.cpp.o"

# External object files for target lex
lex_EXTERNAL_OBJECTS =

lex: CMakeFiles/lex.dir/lex.cpp.o
lex: CMakeFiles/lex.dir/build.make
lex: libdfa.a
lex: libnfa.a
lex: libcompiler.a
lex: libhash.a
lex: libset.a
lex: CMakeFiles/lex.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/sheep/Compiler/cx/lex/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable lex"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lex.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lex.dir/build: lex
.PHONY : CMakeFiles/lex.dir/build

CMakeFiles/lex.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lex.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lex.dir/clean

CMakeFiles/lex.dir/depend:
	cd /home/sheep/Compiler/cx/lex/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sheep/Compiler/cx/lex /home/sheep/Compiler/cx/lex /home/sheep/Compiler/cx/lex/build /home/sheep/Compiler/cx/lex/build /home/sheep/Compiler/cx/lex/build/CMakeFiles/lex.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/lex.dir/depend


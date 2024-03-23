# CMake generated Testfile for 
# Source directory: /home/sheep/Compiler/cx/toolcpp
# Build directory: /home/sheep/Compiler/cx/build/toolcpp
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[settest]=] "/home/sheep/Compiler/cx/build/toolcpp/setTest")
set_tests_properties([=[settest]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/sheep/Compiler/cx/toolcpp/CMakeLists.txt;45;add_test;/home/sheep/Compiler/cx/toolcpp/CMakeLists.txt;0;")
add_test([=[hashtest]=] "/home/sheep/Compiler/cx/build/toolcpp/hashTest")
set_tests_properties([=[hashtest]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/sheep/Compiler/cx/toolcpp/CMakeLists.txt;54;add_test;/home/sheep/Compiler/cx/toolcpp/CMakeLists.txt;0;")
subdirs("../_deps/googletest-build")

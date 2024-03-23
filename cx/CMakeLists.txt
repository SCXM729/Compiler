cmake_minimum_required(VERSION 3.28)
project(com)
# cmake --test-dir build --parallel 4 -V
enable_testing()
add_subdirectory(lexcpp)
add_subdirectory(toolcpp)

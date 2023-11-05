cmake_minimum_required(VERSION 3.27)
project(test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(set STATIC ../tool/set.h ../tool/set.cpp)

add_executable(setTest setTest.cpp)
target_link_libraries(setTest set)


add_executable(hashTest hashTest.cpp)


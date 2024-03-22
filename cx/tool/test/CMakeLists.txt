cmake_minimum_required(VERSION 3.27)
project(test LANGUAGES CXX)

include_directories(~/googletest/build/install/include)
link_directories(~/googletest/build/install/lib)

add_compile_options(-DDEBUG)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(set STATIC ../set.h ../set.cpp)
add_library(compiler STATIC ../compiler.h ../compiler.cpp)
add_library(hash STATIC ../hash.h ../hash.cpp)

add_executable(setTest setTest.cpp)
target_link_libraries(setTest set gtest gtest_main gmock gmock_main)




# add_executable(hashTest hashTest.cpp)
# target_link_libraries(hashTest hash compiler)

# add_executable(sortTest sortTest.cpp)
# target_link_libraries(sortTest compiler)

# add_executable(compilerTest compilerTest.cpp)
# target_link_libraries(compilerTest compiler gtest gtest_main gmock gmock_main)

# add_executable(debugcompilerTest debugcompilerTest.cpp)
# target_link_libraries(debugcompilerTest compiler)

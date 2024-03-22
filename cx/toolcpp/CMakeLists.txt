cmake_minimum_required(VERSION 3.28)
project(toolcpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE DEBUG)

add_compile_options(-DDEBUG)
add_compile_options(-Wall -Werror)
# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

# include googletest
include_directories(~/googletest/build/install/include)
link_directories(~/googletest/build/install/lib)

# set library
add_library(set STATIC)
target_sources(set PRIVATE set.cpp)

# compiler library
add_library(compiler STATIC)
target_sources(compiler PRIVATE compiler.cpp)

# hash library
add_library(hash STATIC)
target_sources(hash PRIVATE hash.hpp)
set_target_properties(hash PROPERTIES LINKER_LANGUAGE CXX)

# # integrate all library 
# add_library(tools STATIC)
# target_sources(tools PRIVATE set.cpp compiler.cpp hash.hpp)
# target_link_libraries(tools set compiler)

# just for test library 
add_executable(setTest)
target_sources(setTest PRIVATE setTest.cpp)
target_link_libraries(
    setTest
    PRIVATE set gtest gtest_main gmock gmock_main
)

add_executable(setCompiler compilerTest.cpp)
target_link_libraries(
    setCompiler
    compiler
    gtest
    gtest
    gtest_main
    gmock
    gmock_main
)

add_executable(hashTest hash.hpp hashTest.cpp)
target_link_libraries(
    hashTest
    compiler
    gtest
    gtest_main
    gmock
    gmock_main
)

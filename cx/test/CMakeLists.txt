cmake_minimum_required(VERSION 3.28)
project(lexcpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE DEBUG)


find_library(tools NAMES  libcompiler.a libset.a HINTS ../toolcpp/build)
find_library(tools NAMES  libcompiler.a libset.a HINTS ../toolcpp/build)
add_compile_options(-DDEBUG)

include_directories(~/googletest/build/install/include)
link_directories(~/googletest/build/install/lib)

# add_library(set ../toolcpp/set.cpp ../toolcpp/set.h)
# add_library(compiler ../toolcpp/compiler.h ../toolcpp/compiler.cpp)
#add_library(lexio lexio.cpp lexio.h nfa.h nfa.cpp)
#add_library(lexio)
#target_sources(lexio PUBLIC lexio.cpp nfa.cpp)
#target_link_libraries(lexio ${tools})
#target_include_directories(lexio PUBLIC ../toolcpp)

#add_executable(lex)
#target_sources(lex PUBLIC lex.cpp)
#target_include_directories(lex PUBLIC ../toolcpp)

#target_link_libraries(lex  lexio ${tools} gtest gtest_main gmock gmock_main)

add_executable(main)
target_sources(main PUBLIC main.cpp)
target_link_libraries(main ${tools})
target_include_directories(main PUBLIC ../toolcpp)

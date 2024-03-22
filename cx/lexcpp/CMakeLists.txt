cmake_minimum_required(VERSION 3.28)
project(lexcpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE DEBUG)

# include googletest
include_directories(~/googletest/build/install/include)
link_directories(~/googletest/build/install/lib)

# find set compiler hash library 
# find_library(tools NAMES libtools.a HINTS ../toolcpp/build)
# add_compile_options(-DDEBUG)

# add_library(set ../toolcpp/set.cpp ../toolcpp/set.h)
# add_library(compiler ../toolcpp/compiler.h ../toolcpp/compiler.cpp)
#add_library(lexio lexio.cpp lexio.h nfa.h nfa.cpp)
add_library(lexio)
target_sources(
    lexio
    PUBLIC
        lexio.cpp
        nfa.cpp
        nfaThompson.cpp
        printNfa.cpp
        dfa.cpp
        dfaTerp.cpp
        minimize.cpp
)
target_link_libraries(lexio set compiler hash)
target_include_directories(lexio PUBLIC ../toolcpp)

add_executable(lex)
target_sources(lex PUBLIC lex.cpp)
target_include_directories(lex PUBLIC ../toolcpp)

target_link_libraries(
    lex
    lexio
    set
    hash
    compiler
    # ${tools}
    gtest
    gtest_main
    gmock
    gmock_main
)
add_custom_command(
    TARGET lex
    POST_BUILD
    COMMAND
        valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
        ./lex
    COMMENT "memory check "
)

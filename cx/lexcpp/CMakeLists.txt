cmake_minimum_required(VERSION 3.28)
project(lexcpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

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

target_link_libraries(lex lexio set hash compiler)
include(CTest)
enable_testing()
find_program(MEMORYCHECK_COMMAND NAMES valgrind)
set(MEMORYCHECK_COMMAND_OPTIONS
    "--track-origins=yes --leak-check=full --show-leak-kinds=all"
)
add_test(NAME lexMem COMMAND $<TARGET_FILE:lex>)

add_custom_target(
    c.lex
    COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c.lex
        ${CMAKE_CURRENT_BINARY_DIR}/c.lex
)
add_dependencies(lex c.lex)

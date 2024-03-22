cmake_minimum_required(VERSION 3.28)
project(LEX)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-DDEBUG)
add_library(set STATIC ../tool/set.h ../tool/set.cpp)
add_library(hash STATIC ../tool/hash.h ../tool/hash.cpp)
add_library(compiler STATIC ../tool/compiler.h ../tool/compiler.cpp pairs.cpp print_ar.cpp)

#add_library(terp STATIC )
add_library(nfa STATIC nfa.h globals.h ../tool/stack.h ../tool/compiler.h ../tool/debug.h ../tool/hash.h ../tool/set.h nfa.cpp printnfa.cpp terp.cpp)
target_link_libraries(nfa set compiler hash )

add_library(dfa STATIC dfa.h ../tool/compiler.h ../tool/debug.h ../tool/set.h globals.h nfa.h dfa.cpp input.cpp minimize.cpp print.cpp signon.cpp squash.cpp)
target_link_libraries(dfa nfa set)


add_executable(lex globals.h ../tool/compiler.h nfa.h dfa.h lex.cpp)
target_link_libraries(lex dfa nfa)

add_executable(lext globals.h ../tool/compiler.h nfa.h dfa.h lext.cpp)
target_link_libraries(lext dfa nfa)


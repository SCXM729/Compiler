set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE DEBUG)

add_compile_options(-DDEBUG)
add_compile_options(-Wall -Werror)
# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

include(FetchContent)
fetchcontent_declare(
    googletest
    URL
        https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# prevent googletest from overriding out compiler/linker options when build with VS
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
fetchcontent_makeavailable(googletest)
# include googletest
# include_directories(~/googletest/build/install/include)
# link_directories(~/googletest/build/install/lib)

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

include(CTest)
enable_testing()
include(GoogleTest)

# just for test library 
add_executable(setTest)
target_sources(setTest PRIVATE setTest.cpp)
target_link_libraries(setTest PRIVATE set GTest::gtest_main GTest::gmock_main)

add_test(NAME settest COMMAND $<TARGET_FILE:setTest>)

add_executable(hashTest)
target_sources(hashTest PRIVATE hashTest.cpp)
target_link_libraries(
    hashTest
    PRIVATE hash compiler GTest::gtest_main GTest::gmock_main
)

add_test(NAME hashtest COMMAND $<TARGET_FILE:hashTest>)

set (CMAKE_CPP_COMPILER g++)

# the debug flag is used by gdb
#set (CMAKE_BUILD_TYPE "Debug")

set (CMAKE_CXX_FLAGS_LIST 
        -g -Wall -Wextra -Wunused-variable -std=c++11 -pthread -pipe -D_GLIBCXX_USE_NANOSLEEP 
    )
string (REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_LIST}")

# src files
set (SRC_LIST 
    sysinfo/hardware.cpp
    utils/runnable.cpp
    )
set (MICRO_LIST
    utils/microbench.c
    )

# test files
set (TEST_DIR ../test)

set (TEST_LIST
    ${TEST_DIR}/test_hardware.cpp
    )

# build test
add_executable (microbench ${MICRO_LIST})
target_link_libraries (microbench pthread)

add_executable (test_all ${TEST_LIST} ${SRC_LIST})
target_link_libraries (test_all gtest gtest_main) # link library: libxxx.a

# set output path as build/bin
set (EXECUTABLE_OUTPUT_PATH ../bin)

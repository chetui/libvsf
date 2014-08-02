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
    sysinfo/define_func_option.cpp
    utils/func_option.cpp
    utils/runnable.cpp
    vsf.cpp
    )
set (MICRO_LIST
    utils/microbench.c
    )

# test files
set (TEST_DIR ../test)

set (TEST_LIST
    ${TEST_DIR}/test_hardware.cpp
    ${TEST_DIR}/test_func_option.cpp
    ${TEST_DIR}/test_vsf.cpp
    )

# example files
set (EXAMPLE_DIR ../example)

# build tests
add_executable (microbench ${MICRO_LIST})
target_link_libraries (microbench pthread)

    #build test_all
add_executable (test_all ${TEST_LIST} ${SRC_LIST})
target_link_libraries (test_all gtest gtest_main) # link library: libxxx.a

    #build individual tests
    add_executable (test_hardware ${TEST_DIR}/test_hardware.cpp ${SRC_LIST})
    target_link_libraries (test_hardware gtest gtest_main)
    add_executable (test_func_option ${TEST_DIR}/test_func_option.cpp ${SRC_LIST})
    target_link_libraries (test_func_option gtest gtest_main)
    add_executable (test_vsf ${TEST_DIR}/test_vsf.cpp ${SRC_LIST})
    target_link_libraries (test_vsf gtest gtest_main)

# build examples
add_executable (implemented_example ${EXAMPLE_DIR}/implemented_example.cpp ${SRC_LIST})

# set output path as build/bin
set (EXECUTABLE_OUTPUT_PATH ../bin)

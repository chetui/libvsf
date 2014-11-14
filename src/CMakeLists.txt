set (CMAKE_CPP_COMPILER g++)
# set output path as build/bin
set (EXECUTABLE_OUTPUT_PATH ../bin)
# test dir
set (TEST_DIR ../test)
# example dir
set (EXAMPLE_DIR ../example)

# the debug flag is used by gdb
set (CMAKE_BUILD_TYPE "Debug")
set (CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb3 -DDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")

set (CMAKE_CXX_FLAGS_LIST 
        -Wall -Wextra -Wunused-variable -std=c++14 -pthread -pipe -lnuma -D_GLIBCXX_USE_NANOSLEEP 
    )
string (REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_LIST}")

# src files
set (SRC_LIST 
    sysinfo/host/static/node_cpu.cpp
    sysinfo/host/static/node_sys_dist.cpp
    sysinfo/host/static/node_test_dist.cpp
    sysinfo/host/static/total_mem_size.cpp
    sysinfo/host/dymatic/hs_cpu_usage.cpp
    sysinfo/host/dymatic/hs_used_mem_size.cpp
    sysinfo/vm/static/vm_node_info.cpp
    sysinfo/vm/dynamic/vm_base.cpp
    sysinfo/vm/dynamic/vm_cpu_usage.cpp
    sysinfo/vm/dynamic/cpu_usage.cpp
    sysinfo/vm/dynamic/cache_miss.cpp
    sysinfo/vm/dynamic/vm_cache_miss.cpp
    exec/cpu_mig.cpp
    utils/func_option.cpp
    utils/runnable.cpp
    utils/log.cpp
    utils/str_tools.cpp
    utils/perf_event.cpp
    framework/host.cpp
    framework/vm.cpp
    framework/exec.cpp
    framework/compat.cpp
    framework/version.cpp
    vsf.cpp
    )


# build test_all ---------------------------------
#set (TEST_LIST
#    )
#add_executable (test_all ${TEST_LIST} ${SRC_LIST})
#target_link_libraries (test_all gtest gtest_main) # link library: libxxx.a

# build individual tests ------------------------
macro (add_indi_test test_name)
    add_executable (${test_name} ${TEST_DIR}/${test_name}.cpp ${SRC_LIST})
    target_link_libraries (${test_name} gtest gtest_main proc)
endmacro ()

add_indi_test (test_host_node_cpu)
add_indi_test (test_host_node_sys_dist)
add_indi_test (test_host_node_test_dist)
add_indi_test (test_host_total_mem_size)
add_indi_test (test_host_cpu_usage)
add_indi_test (test_host_used_mem_size)
add_indi_test (test_vm_vm_base)
add_indi_test (test_vm_vm_cpu_usage)
add_indi_test (test_vm_cpu_usage)
add_indi_test (test_vm_vm_cache_miss)
add_indi_test (test_vm_cache_miss)
add_indi_test (test_vm_node_info)
add_indi_test (test_exec_cpu_mig)
add_indi_test (test_func_option)
add_indi_test (test_vsf_init_twice)
add_indi_test (test_vsf_exec_cpu_mig)
add_indi_test (test_vsf_vm_base)
add_indi_test (test_vsf_vm_cpu_usage)
add_indi_test (test_vsf_vm_cache_miss)
add_indi_test (test_vsf_node_cpu_node_dist)
add_indi_test (test_compat)

# build examples -------------------------------
add_executable (implemented_example ${EXAMPLE_DIR}/implemented_example.cpp ${SRC_LIST}) 
target_link_libraries (implemented_example proc)
add_executable (vm_scheduling_monitor ${EXAMPLE_DIR}/vm_scheduling_monitor.cpp ${SRC_LIST})
target_link_libraries (vm_scheduling_monitor proc)
add_executable (web_user_generate ${EXAMPLE_DIR}/web_user_generate.cpp ${SRC_LIST})
target_link_libraries (web_user_generate proc)
add_executable (naive_scheduler ${EXAMPLE_DIR}/naive_scheduler.cpp ${SRC_LIST}) 
target_link_libraries (naive_scheduler proc)
add_executable (web_monitor ${EXAMPLE_DIR}/web_monitor.cpp ${SRC_LIST}) 
target_link_libraries (web_monitor proc)

# build other dep exec ------------------------
add_executable (microbench utils/microbench.c)
target_link_libraries (microbench pthread)


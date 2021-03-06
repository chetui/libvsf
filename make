#!/bin/bash

set -o nounset    
set -o errexit

OUTPUT_DIR=./build/

rm -rf ${OUTPUT_DIR}/*
cd ${OUTPUT_DIR}
echo ""
echo "==================<<<Build1: CMake>>>===================="
echo ""
cmake ..
echo ""
echo "==================<<<Build2: Make>>>====================="
echo ""
make -j 17
echo ""
cp ../example/get_vcpu_affinity.py ./bin
cp ../example/init_scheduling_result.py ./bin
cp ../example/get_scheduling_result.py ./bin
cp -r ../include ./libvsf
mkdir ./libvsf/bin
cp ./bin/microbench ./libvsf/bin

if  [ "$#" == "1" ] && [ "$1" == "test" ]; then
    echo "==================<<<UnitTest: GTest>>>=================="
    echo ""
    cd ./bin
    ./test_vsf_init_twice
    ./test_vsf_exec_cpu_mig
    ./test_vsf_vm_base
    ./test_vsf_vm_cpu_usage
    ./test_vsf_vm_cache_miss
    ./test_vsf_node_cpu_node_dist
    ./test_vm_vm_base
    ./test_vm_vm_cpu_usage
    ./test_vm_cpu_usage
    ./test_vm_vm_cache_miss
    ./test_vm_cache_miss
    ./test_exec_cpu_mig
    ./test_func_option
    ./test_compat
    ./test_host_node_cpu
    ./test_host_node_sys_dist
    ./test_host_node_test_dist
    ./implemented_example
    #./test_all
fi
#echo "========<<<CodeStyleCheck: cpplint>>>========="

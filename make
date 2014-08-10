#!/bin/bash

set -o nounset    
set -o errexit

rm -rf ./build/*
cd ./build
echo ""
echo "==================<<<Build1: CMake>>>===================="
echo ""
cmake ..
echo ""
echo "==================<<<Build2: Make>>>====================="
echo ""
make -j 17
echo ""

if  [ "$#" == "1" ] && [ "$1" == "test" ]; then
    echo "==================<<<UnitTest: GTest>>>=================="
    echo ""
    cd ./bin
    ./test_vsf_0
    ./test_vsf_1
    ./test_vsf_2
    ./test_vm_vm_base
    ./test_func_option
    ./test_compat
    ./test_host_node_core_hpthread
    ./test_host_node_dist
    ./implemented_example
    #./test_all
fi
#echo "========<<<CodeStyleCheck: cpplint>>>========="

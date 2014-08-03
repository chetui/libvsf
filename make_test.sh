#!/bin/bash

rm -rf ./build/*
cd ./build
echo ""
echo "==================<<<Build1: CMake>>>===================="
echo ""
cmake ..
echo ""
echo "==================<<<Build2: Make>>>====================="
echo ""
make
echo ""
echo "==================<<<UnitTest: GTest>>>=================="
echo ""
cd ./bin
./test_host_node_core_hpthread
./test_host_node_dist
./test_func_option
./test_vsf
#./test_all
#echo "========<<<CodeStyleCheck: cpplint>>>========="

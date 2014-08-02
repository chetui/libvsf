#!/bin/bash

rm -rf ./build/*
cd ./build
echo "==================<<<Build1: CMake>>>===================="
cmake ..
echo "==================<<<Build2: Make>>>====================="
make
echo "==================<<<UnitTest: GTest>>>=================="
cd ./bin
./test_hardware
./test_func_option
./test_vsf
#./test_all
#echo "========<<<CodeStyleCheck: cpplint>>>========="

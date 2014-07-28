#!/bin/bash

rm -rf ./build/*
cd ./build
cmake ..
make
./bin/test_all

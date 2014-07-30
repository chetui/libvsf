cmake_minimum_required (VERSION 2.8)

project (scheduler)

include_directories (include)
# subdir must also have CMakeLists.txt
add_subdirectory (src)
add_subdirectory (test/tools/gtest)

# to enable [make test] cmd, which would execute target [test_all]
# notice [make test] is the Unit Test framework of Make, rather than GTest. 
# to use GTest, please run test_all binary file.
enable_testing()
add_test (NAME test COMMAND test_all)

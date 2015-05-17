libvsf
======

a library for Virtual Machine Scheduling Framework.

#### Dependency

* g++-4.9.0 or later (with C++14 support)
* cmake 2.8.7 or later
* procps 1:3.2.8-11

#### How to build & install

```
$ ./make
```

Then copy the directory ./build/libvsf to your source directory.  
Assume you have a program named my_program using libvsf.

```
$ g++ my_program.cpp -o my_program -I./libvsf/include -L./libvsf -lpthread -lproc -lvsf -std=c++14
```

#### Examples

* [Implemented Example](https://github.com/chetui/libvsf/blob/master/example/implemented_example.cpp)

* There is another example [Base Example](https://github.com/chetui/libvsf/blob/master/example/base_example.cpp), which contains all the new features we are working on but not available yet.

#### Dev Guide 

[Sysinfo Development Guide](https://github.com/chetui/libvsf/blob/master/doc/sysinfo_dev_guide.md)  
Project Tools: Gtest, cpplint.

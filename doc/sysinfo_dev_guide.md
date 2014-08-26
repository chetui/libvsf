Sysinfo Development Guide
======
[This doc is out of date currently]
The purpose of this guide is to help the developers who want to add new sysinfo class into libvsf.

## General Requirement

#### the name of API

The final API is just the name of the sysinfo data, like *int node_num()*.  
The name of sysinfo class public function is get\_\*(), like *int get_node_num()*.

#### input parameter type of sysinfo class public function

If input parameter is **fundamental types**, such as *int* and *double*, **use copy**. For example, *int get_node_id(int core_id)*.  
If input parameter is **class types**, such as *class*, *struct* and *union*, **use const reference**, which can imporve performance. For example, *int get_cpu_usage(const VcpuId &vcpu_id)*.

#### return type of sysinfo class public function

Return type must be copy, rather than reference or pointer, in case of modification by users.

#### config parameter of sysinfo class public function

Config parameter is optional.   
If the result of your sysinfo is based on user-defined config, then you need to add config parameter into your design.

The logic of config parameter:
* Users can execute function by different config parameters; 
* When users execute function by the same config parameters, function would return the cached result produced by the first execution, namely always return the same result.

The implementation of config parameter:
* Add a new OptionParam to sysinfo/define_func_option.h;
* Add code in *init_host()* (actually in Host::Host()) or *init_vms()*;
* Add config parameter version of sysinfo class public function && API;

#### multi-thread safety

The sysinfo class must be multi-thread safety.   
It means you need to consider all the data in the sysinfo class, to check whether they are multi-thread safety.  

In my practice, you need to consider each member variable one by one. 
* Add corresponding *mutex* to STL container; Add *lock_guard<mutex>* in each functions who read or write STL container;
* Add *atomic<...>* to **fundamental types**;
* Some variables would always be read or written along with STL container protected by *mutex*. Hence they do not need *atomic<...>* or *mutex*;
* Do not recurse *lock_guard<mutex>* bound by the same *mutex* variable, to avoid dead lock;
* Use the same order of all the *mutex* variables when define multiple *lock_guard<mutex>*, to avoid dead lock;

Sysinfo Development Guide
======

The purpose of this guide is to help the developers who want to add new sysinfo module into libvsf.

## General Requirement

#### the naming rule

For API:  
The final API is just the name of the sysinfo data, like *int node_num()*.  
The name of sysinfo class public function is get\_\*(), like *int get_node_num()*.
  
For class name and file name:  
Class name, file name, Options name should be consistent.  
E.g,  
for host related modules: class SomeModule, some\_module.cpp, some\_module.h, Option::OP_HS_SOME_MODULE;   
for vm related modules: class VmSomeModule, vm\_some\_module.cpp, vm\_some\_module.h, Option::OP_VM_SOME_MODULE.

#### input parameter type of sysinfo class public function

If input parameter is **fundamental types**, such as *int* and *double*, **use copy**. For example, *int get_node_id(int core_id)*.  
If input parameter is **class types**, such as *class*, *struct* and *union*, **use const reference**, which can imporve performance. For example, *int get_cpu_usage(const VcpuId &vcpu_id)*.

#### return type of sysinfo class public function

Return type must be copy, rather than reference or pointer, in case of modification by users.

#### config parameter of sysinfo class public function

Config parameter is optional.   
If the result of your sysinfo is based on user-defined config, then you need to add config parameter into your design.

The logic of config parameter:
* Users can set config parameter by set\_*().
* Users can reset all the config paramters into default values by *clear\_param()*.
* Notice *clear()* is only for data clean, it should not change the config paramters.
* For static info, when users execute function by the same config parameters, function would return the cached result produced by the first execution, namely always return the same result.

The implementation of config parameter:
* Add a new OptionParam to sysinfo/define_func_option.h;
* Add code in *init_host()* (actually in *Host::Host()*) or *init_vms()* (actually in *VmSet::set_param_by_option()*);
* Add code in *set\_param()* & *clear\_param()* in Host or VmSet;
* Add config parameter version of sysinfo class public function & API;
* Add code to *clear\_param()* of module;
* Notice any read/write of config parameter in sysinfo class should be thread-safe;
* For callback paramter, you should add a new micro in *define\_func\_option.h*, and corresponding clean code in *VmSet::delete\_callback()* & *VmSet::~VmSet()*;

#### multi-thread safety

The sysinfo class must be multi-thread safety.   
It means you need to consider all the data in the sysinfo class, to check whether they are multi-thread safety.  

In my practice, you need to consider each member variable one by one. 
* Add corresponding *shared_timed_mutex* to STL container; 
* Add *unique_lock<shared_timed_mutex>* in each functions who write STL container;
* Add *shared_lock<shared_timed_mutex>* in each functions who read STL container;
* If you have multiple STL containers, in most cases you need only one *shared_timed_mutex* to protect them all.
* Add *atomic<...>* to **fundamental types**;
* Some variables would always be read or written along with STL container protected by *mutex*. Hence they do not need *atomic<...>* or *mutex*;
* Do not recurse any *lock* bound by the same *mutex* variable except *recursive_mutex*, to avoid dead lock;
* Use the same order of all the *mutex* variables when define multiple *lock*, to avoid dead lock;

#### callback function

For any sysinfo module that can be run as a thread asynchronously, you must define callback function to allow users do something synchronously when data is refreshed.

#### data clean

In any sysinfo module, there should be a *clear()* function to clean the data.  
For dynamic module, the *clear()* should be called by the end of *refresh()*;  
For static module, the *clear()* should be public to allow user to clear the data.  

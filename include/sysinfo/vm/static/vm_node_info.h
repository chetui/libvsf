#ifndef __VM_NODE_INFO_H__
#define __VM_NODE_INFO_H__
#include<string>
#include<iostream>
#include<vector>
#include<atomic>
#include<mutex>
using namespace std;

struct vm_vnode_static_info
{
    int vnode_id;
    vector<int> cpus;
    int mem_size_mb;
    int static_bind;
};

struct vm_vnode_return_structure
{
    int vnode_size;
    vector<vm_vnode_static_info> vnode_info;
};

const int BUF_SIZE=10240;


class vmNodeInfo
{
    int vnode_num;
    vector<vm_vnode_static_info> vnode_info;
    atomic<bool> init;
    mutex mtx;
    uint32_t pid;
    vmNodeInfo(uint32_t pid):vnode_num(-1),init(false),pid(pid){};
public:
    static vmNodeInfo * get_instance(uint32_t pid);

    int update();
    int get_vnode_num();
    vector<int> get_vnode_ids();
    vector<int> get_vcpu_ids(int vnode_id,int vcpu_num=0);
};
#endif

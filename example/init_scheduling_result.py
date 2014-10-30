import subprocess, json


vm_id = {}
def get_vm_index(vmid):
    if vmid in vm_id.keys():
        return vm_id[vmid]
    else:
        vm_id[vmid] = len(vm_id) + 2
        return vm_id[vmid]

if __name__ == '__main__':
    p = subprocess.Popen("./vm_scheduling_monitor",
                     stdout = subprocess.PIPE,
                     stderr = subprocess.PIPE,
                     shell = True
    )

    res_string = p.stdout.readlines()

    f = open("last.txt", "w")
    f.writelines(res_string)
    f.close();

    
    #print json.loads(res_string[0])
    #print json.loads(res_string[1])

    hpthreads_json = json.loads(res_string[0])
    vms_json = json.loads(res_string[1])

    nodes = []
    links = []

    hpthreads_indexes = {}
    vcpus_indexes = {}
    for n in hpthreads_json:
        node_index = len(nodes)
        nodes.append({"name": "node" + str(n['nodeid']), "group": 0})
        for h in n['hpthreads']:
            hpthreads_indexes[h] = len(nodes)
            nodes.append({"name": "hpthread" + str(h), "group": 1})
            links.append({"source":node_index, "target":hpthreads_indexes[h], "value":1})

    for v in vms_json:
        name = v['vmname']
        cnt = 0;
        for vcpu in v['vcpus']:
            v_index = len(nodes)
            vcpus_indexes[vcpu['vcpuid']] = v_index
            nodes.append({"name": name + ":" + str(vcpu['vcpuid']),
                          "group": get_vm_index(name),
                          "affinity": v['affinity'][cnt]})
            cnt = cnt + 1
            on = vcpu['runningon']
            links.append({"source": v_index, "target": hpthreads_indexes[on], "value":1})

    res = {"nodes": nodes, "links":links}
    print json.dumps(res)


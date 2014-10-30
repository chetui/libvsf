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

    f = open("last.txt", "r")
    last_res_string = f.readlines()
    f.close()
    
    f = open("last.txt", "w")
    f.writelines(res_string)
    f.close();
    
    vms_json = json.loads(res_string[1])
    last_vms_json = json.loads(last_res_string[1])

    vms_hpt_map = {}
    for v in last_vms_json:
        for vcpu in v['vcpus']:
            on = vcpu['runningon']
            vms_hpt_map[vcpu['vcpuid']] = on

    operations = []
    for v in vms_json:
        name = v['vmname']
        for vcpu in v['vcpus']:
            on = vcpu['runningon']
            old_on = vms_hpt_map[vcpu['vcpuid']]
            if old_on!= on:
                operations.append({"operation": "move",
                                   "vcpu": name + ":" + str(vcpu['vcpuid']),
                                   "hpthread": "hpthread" + str(on)})
                # operations.append({"operation": "delete_link",
                #                    "source": "hpthread" + str(old_on),
                #                    "target": name + ":" + str(vcpu['vcpuid'])})
                # operations.append({"operation": "add_link",
                #                    "source": name + ":" + str(vcpu['vcpuid']),
                #                    "target": "hpthread" + str(on)})
    
    print json.dumps(operations)


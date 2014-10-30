#include "vsf.h"

Vsf* vsf = Vsf::get_instance();
void myscheduler(Host *host, std::set<VM> &vms)
{
    // fill your scheduling algorithm here
    // you can bound some vcpus to hyperthread or set its affinity
}
   
int main()
{
    //set some flags ahead of time to refresh optional functions info when init_host(), init_vms(), update_info();
    //if others functions are called without set corresponding flags, info would be collected immediately.
    vsf->init({

});
    //refresh <<Optional Host Static Info>>
    Host *host = vsf->init_host();
    
    while(true) {

        //refresh <<Optional VM Static Info>>
        //and start threads of <<Optional VM Dynamic Info>>
        std::set<VM> vms = vsf->init_vms(host);

        //your scheduler algorithm
        myscheduler(host, vms);

        vsf->exec_mig(); //only host parameter may be OK?

        sleep(1); 
    }

    return 0;
}

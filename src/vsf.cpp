#include "vsf.h"
#include "utils/log.h"

using namespace std;

Vsf::Vsf()
{
    func_option_ = FuncOption::get_instance();
    vm_set_ = VmSet::get_instance();
    exec_ = Exec::get_instance();
}

Vsf* Vsf::get_instance()
{
    static Vsf vsf;
    return &vsf;
}

void Vsf::set_param(std::map<Option, std::map<OptionParam, OptionParamVal> > ops)
{
    for (auto& op : ops) {
        if (op.first > Option::SEPARATOR_FOR_HS_AND_VM) {
            vm_set_->set_param({op});
        } else {
            host_->set_param({op});
        }
    }
//    LDEBUG << "check_option:" << func_option_->check_option(Option::OP_HS_NODE_TEST_DIST);
//    for (auto& op : ops) {
//    if (op.first == Option::OP_HS_NODE_TEST_DIST) {
//        LDEBUG << "before A";
//        map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_HS_NODE_TEST_DIST);
//        for (auto & p : param) 
//        {
//            LDEBUG << "before B";
//            switch(p.first)
//            {
//            case OptionParam::PATH:
//                LDEBUG << "before" << p.second.get_string();
//                break;
//            case OptionParam::SIZE_IN_MB:
//                LDEBUG << "before" << p.second.get_int();
//                break;
//            case OptionParam::WORKLOAD_TYPE:
//                LDEBUG << "before" << p.second.get_char();
//                break;
//            case OptionParam::LOOP:
//                LDEBUG << "before" << p.second.get_int();
//                break;
//            default:
//                //cerr message "invalid parameter for OP_HS_NODE_TEST_DIST"
//                break;
//            }
//        }
//    }
//    }
//    for (auto& op : ops) {
//    if (op.first == Option::OP_HS_NODE_TEST_DIST) {
//        map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_HS_NODE_TEST_DIST);
//        for (auto & p : param) 
//        {
//            switch(p.first)
//            {
//            case OptionParam::PATH:
//                LDEBUG << "after" << p.second.get_string();
//                break;
//            case OptionParam::SIZE_IN_MB:
//                LDEBUG << "after" << p.second.get_int();
//                break;
//            case OptionParam::WORKLOAD_TYPE:
//                LDEBUG << "after" << p.second.get_char();
//                break;
//            case OptionParam::LOOP:
//                LDEBUG << "after" << p.second.get_int();
//                break;
//            default:
//                //cerr message "invalid parameter for OP_HS_NODE_TEST_DIST"
//                break;
//            }
//        }
//    }
//    }
}

void Vsf::clear_param(std::initializer_list<Option> ops)
{
    for (auto& op : ops) {
        if (op > Option::SEPARATOR_FOR_HS_AND_VM)
            vm_set_->clear_param({op});
        else
            host_->clear_param({op});
    }
}

//framework
void Vsf::init(std::map<Option, std::map<OptionParam, OptionParamVal> > ops)
{
    static int inited_cnt = 0;
    if(++inited_cnt > 1)
        THROW(FrameworkInitMoreThanTwice, "Vsf is inited twice.");

    func_option_->enable_option(ops);
//    for (auto& op : ops) {
//    if (op.first == Option::OP_HS_NODE_TEST_DIST) {
//        map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_HS_NODE_TEST_DIST);
//        for (auto & p : param) 
//        {
//            switch(p.first)
//            {
//            case OptionParam::PATH:
//                LDEBUG << "after" << p.second.get_string();
//                break;
//            case OptionParam::SIZE_IN_MB:
//                LDEBUG << "after" << p.second.get_int();
//                break;
//            case OptionParam::WORKLOAD_TYPE:
//                LDEBUG << "after" << p.second.get_char();
//                break;
//            case OptionParam::LOOP:
//                LDEBUG << "after" << p.second.get_int();
//                break;
//            default:
//                //cerr message "invalid parameter for OP_HS_NODE_TEST_DIST"
//                break;
//            }
//        }
//    }
//    }
    return;
}

Host *Vsf::init_host()
{
    host_ = Host::get_instance();
    return host_;
}

set<VM> Vsf::init_vms(Host *host, string vm_cmd)
{
    return vm_set_->init_vms(host, vm_cmd);
}

set<VM> Vsf::init_vms(Host *host)
{
    return vm_set_->init_vms(host);
}

void Vsf::exec_mig()
{
    exec_->exec_mig();
}

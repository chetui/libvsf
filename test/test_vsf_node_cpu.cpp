#include <iostream>
#include <set>
#include "gtest/gtest.h"

#include "vsf.h"

using namespace std;

class VsfTest : public ::testing::Test {
protected:
    VsfTest() {
        vsf = Vsf::get_instance();
    }

    Vsf* vsf;
};

TEST_F(VsfTest, node_num_and_node_dist)
{
    vsf->init({ 
        { Option::OP_HS_NODE_CPU, {} },
        { Option::OP_HS_SYS_NODE_DIST, { } },
        { Option::OP_HS_TEST_NODE_DIST, 
            { 
                { OptionParam::PATH, "." },
                { OptionParam::SIZE_IN_MB, 21 },
                { OptionParam::WORKLOAD_TYPE, WORKLOADTYPE_RANDOM },
                { OptionParam::LOOP, 210 }
            }
         }
    });
    Host *host = vsf->init_host();

    ASSERT_EQ(host->node_num(), 2);
    bool res = host->node_ids() == std::set<NodeId>{ NodeId(0), NodeId(1) };
    for (auto& id : host->node_ids())
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);

    ASSERT_EQ(host->node_id(SocketId(0)), NodeId(1));
    ASSERT_EQ(host->node_id(CoreId(1, 10)), NodeId(0));
    ASSERT_EQ(host->node_id(HpthreadId(14)), NodeId(0));

    ASSERT_EQ(host->socket_num(), 2);
    ASSERT_EQ(host->socket_num(NodeId(0)), 1);
    
    res = host->socket_ids() == std::set<SocketId>{ SocketId(0), SocketId(1) };
    for (auto& id : host->socket_ids())
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);

    res = host->socket_ids(NodeId(1)) == std::set<SocketId>{ SocketId(0) };
    for (auto& id : host->socket_ids(NodeId(1)))
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);

    ASSERT_EQ(host->socket_id(CoreId(1, 10)), SocketId(1));
    ASSERT_EQ(host->socket_id(HpthreadId(9)), SocketId(0));

    ASSERT_EQ(host->core_num(), 8);
    ASSERT_EQ(host->core_num(NodeId(1)), 4);
    ASSERT_EQ(host->core_num(SocketId(1)), 4);

    res = host->core_ids() == std::set<CoreId>{ CoreId(0,0), CoreId(0,1), CoreId(0, 9), CoreId(0, 10), CoreId(1, 0), CoreId(1, 1), CoreId(1, 9), CoreId(1, 10) };
    for (auto& id : host->core_ids())
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);

    res = host->core_ids(NodeId(1)) == std::set<CoreId>{ CoreId(0,0), CoreId(0,1), CoreId(0, 9), CoreId(0, 10) };
    for (auto& id : host->core_ids(NodeId(1)))
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);

    res = host->core_ids(SocketId(0)) == std::set<CoreId>{ CoreId(0,0), CoreId(0,1), CoreId(0, 9), CoreId(0, 10) };
    for (auto& id : host->core_ids(SocketId(0)))
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);

    ASSERT_EQ(host->core_id(HpthreadId(4)), CoreId(1, 9));

    ASSERT_EQ(host->hpthread_num(), 16);
    ASSERT_EQ(host->hpthread_num(NodeId(1)), 8);
    ASSERT_EQ(host->hpthread_num(SocketId(1)), 8);
    ASSERT_EQ(host->hpthread_num(CoreId(0, 9)), 2);

    res = host->hpthread_ids() == std::set<HpthreadId>{ HpthreadId(0), HpthreadId(1), HpthreadId(2), HpthreadId(3), HpthreadId(4), HpthreadId(5), HpthreadId(6), HpthreadId(7), HpthreadId(8), HpthreadId(9), HpthreadId(10), HpthreadId(11), HpthreadId(12), HpthreadId(13), HpthreadId(14), HpthreadId(15) };
    for (auto& id : host->hpthread_ids())
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);

    res = host->hpthread_ids(NodeId(0)) == std::set<HpthreadId>{ HpthreadId(0), HpthreadId(2), HpthreadId(4), HpthreadId(6), HpthreadId(8), HpthreadId(10), HpthreadId(12), HpthreadId(14) };
    for (auto& id : host->hpthread_ids(NodeId(0)))
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);

    res = host->hpthread_ids(SocketId(1)) == std::set<HpthreadId>{ HpthreadId(0), HpthreadId(2), HpthreadId(4), HpthreadId(6), HpthreadId(8), HpthreadId(10), HpthreadId(12), HpthreadId(14) };
    for (auto& id : host->hpthread_ids(SocketId(1)))
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);

    res = host->hpthread_ids(CoreId(0, 9)) == std::set<HpthreadId>{ HpthreadId(5), HpthreadId(13) };
    for (auto& id : host->hpthread_ids(CoreId(0, 9)))
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);






    std::cout << host->test_node_dist(0, 1) << std::endl;
    ASSERT_EQ(true, host->test_node_dist(0, 1) > 10);
    std::cout << host->test_node_dist(0, 1, MicroParam(".", 23, WORKLOADTYPE_RANDOM, 230)) << std::endl;
//    ASSERT_EQ(true, host->test_node_dist(0, 1, MicroParam(".", 23, WORKLOADTYPE_RANDOM, 230)) > 10);

    ASSERT_EQ(host->sys_node_dist(0, 1), 20);
    ASSERT_EQ(host->sys_node_dist(1, 1), 10);
}


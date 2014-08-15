#include <set>
#include "gtest/gtest.h"

#include "sysinfo/host/static/node_cpu.h"

using namespace std;

class NodeCpuTest: public ::testing::Test {
protected:
    NodeCpuTest() {
        hd = NodeCpu::get_instance();
        hd->refresh();
    }

    NodeCpu* hd;
};

TEST_F(NodeCpuTest, node_num) 
{
    ASSERT_EQ(hd->get_node_num(), 2);
}

TEST_F(NodeCpuTest, node_ids) 
{
    bool res = hd->get_node_ids() == std::set<NodeId>{ NodeId(0), NodeId(1) };
    for (auto& id : hd->get_node_ids())
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);
}

TEST_F(NodeCpuTest, node_id) 
{
    ASSERT_EQ(hd->get_node_id(SocketId(0)), NodeId(1));
    ASSERT_EQ(hd->get_node_id(CoreId(1, 10)), NodeId(0));
    ASSERT_EQ(hd->get_node_id(HpthreadId(14)), NodeId(0));
}

TEST_F(NodeCpuTest, socket_num) 
{
    ASSERT_EQ(hd->get_socket_num(), 2);
    ASSERT_EQ(hd->get_socket_num(NodeId(0)), 1);
}

TEST_F(NodeCpuTest, socket_ids) 
{
    bool res = hd->get_socket_ids() == std::set<SocketId>{ SocketId(0), SocketId(1) };
    for (auto& id : hd->get_socket_ids())
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);

    res = hd->get_socket_ids(NodeId(1)) == std::set<SocketId>{ SocketId(0) };
    for (auto& id : hd->get_socket_ids(NodeId(1)))
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);
}

TEST_F(NodeCpuTest, socket_id) 
{
    ASSERT_EQ(hd->get_socket_id(CoreId(1, 10)), SocketId(1));
    ASSERT_EQ(hd->get_socket_id(HpthreadId(9)), SocketId(0));
}

TEST_F(NodeCpuTest, core_num) 
{
    ASSERT_EQ(hd->get_core_num(), 8);
    ASSERT_EQ(hd->get_core_num(NodeId(1)), 4);
    ASSERT_EQ(hd->get_core_num(SocketId(1)), 4);
}

TEST_F(NodeCpuTest, core_ids) 
{
    bool res = hd->get_core_ids() == std::set<CoreId>{ CoreId(0,0), CoreId(0,1), CoreId(0, 9), CoreId(0, 10), CoreId(1, 0), CoreId(1, 1), CoreId(1, 9), CoreId(1, 10) };
    for (auto& id : hd->get_core_ids())
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);

    res = hd->get_core_ids(NodeId(1)) == std::set<CoreId>{ CoreId(0,0), CoreId(0,1), CoreId(0, 9), CoreId(0, 10) };
    for (auto& id : hd->get_core_ids(NodeId(1)))
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);

    res = hd->get_core_ids(SocketId(0)) == std::set<CoreId>{ CoreId(0,0), CoreId(0,1), CoreId(0, 9), CoreId(0, 10) };
    for (auto& id : hd->get_core_ids(SocketId(0)))
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);
}

TEST_F(NodeCpuTest, core_id) 
{
    ASSERT_EQ(hd->get_core_id(HpthreadId(4)), CoreId(1, 9));
}

TEST_F(NodeCpuTest, hpthread_num) 
{
    ASSERT_EQ(hd->get_hpthread_num(), 16);
    ASSERT_EQ(hd->get_hpthread_num(NodeId(1)), 8);
    ASSERT_EQ(hd->get_hpthread_num(SocketId(1)), 8);
    ASSERT_EQ(hd->get_hpthread_num(CoreId(0, 9)), 2);
}

TEST_F(NodeCpuTest, hpthread_ids) 
{
    bool res = hd->get_hpthread_ids() == std::set<HpthreadId>{ HpthreadId(0), HpthreadId(1), HpthreadId(2), HpthreadId(3), HpthreadId(4), HpthreadId(5), HpthreadId(6), HpthreadId(7), HpthreadId(8), HpthreadId(9), HpthreadId(10), HpthreadId(11), HpthreadId(12), HpthreadId(13), HpthreadId(14), HpthreadId(15) };
    for (auto& id : hd->get_hpthread_ids())
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);
}

TEST_F(NodeCpuTest, hpthread_ids_node) 
{
    bool res = hd->get_hpthread_ids(NodeId(0)) == std::set<HpthreadId>{ HpthreadId(0), HpthreadId(2), HpthreadId(4), HpthreadId(6), HpthreadId(8), HpthreadId(10), HpthreadId(12), HpthreadId(14) };
    for (auto& id : hd->get_hpthread_ids(NodeId(0)))
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);
}

TEST_F(NodeCpuTest, hpthread_ids_socket) 
{
    bool res = hd->get_hpthread_ids(SocketId(1)) == std::set<HpthreadId>{ HpthreadId(0), HpthreadId(2), HpthreadId(4), HpthreadId(6), HpthreadId(8), HpthreadId(10), HpthreadId(12), HpthreadId(14) };
    for (auto& id : hd->get_hpthread_ids(SocketId(1)))
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);
}

TEST_F(NodeCpuTest, hpthread_ids_core) 
{
    bool res = hd->get_hpthread_ids(CoreId(0, 9)) == std::set<HpthreadId>{ HpthreadId(5), HpthreadId(13) };
    for (auto& id : hd->get_hpthread_ids(CoreId(0, 9)))
        cout << id << ":";
    cout << endl;
    ASSERT_EQ(res, true);
}

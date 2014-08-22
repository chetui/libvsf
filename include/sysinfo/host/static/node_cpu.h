#ifndef _HOST_NODE_CPU_H_
#define _HOST_NODE_CPU_H_

#include <map>
#include <string>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <atomic>

struct NodeId;
struct SocketId;
struct CoreId;
struct HpthreadId;

/**
 * Singlton
 */
class NodeCpu {
public:
    ~NodeCpu();
    /**
     * not thread-safe
     */
    static NodeCpu* get_instance();
    /**
     * Get NUMA node number on this system.
     */
    int get_node_num();
    std::set<NodeId> get_node_ids();
    NodeId get_node_id(SocketId socket_id);
    NodeId get_node_id(CoreId core_id);
    NodeId get_node_id(HpthreadId hpthread_id);
    int get_socket_num();
    int get_socket_num(NodeId node_id);
    std::set<SocketId> get_socket_ids();
    std::set<SocketId> get_socket_ids(NodeId node_id);
    SocketId get_socket_id(CoreId core_id);
    SocketId get_socket_id(HpthreadId hpthread_id);
    int get_core_num();
    int get_core_num(NodeId node_id);
    int get_core_num(SocketId socket_id);
    std::set<CoreId> get_core_ids();
    std::set<CoreId> get_core_ids(NodeId node_id);
    std::set<CoreId> get_core_ids(SocketId socket_id);
    CoreId get_core_id(HpthreadId hpthread_id);
    int get_hpthread_num();
    int get_hpthread_num(NodeId node_id);
    int get_hpthread_num(SocketId socket_id);
    int get_hpthread_num(CoreId core_id);
    std::set<HpthreadId> get_hpthread_ids();
    std::set<HpthreadId> get_hpthread_ids(NodeId node_id);
    std::set<HpthreadId> get_hpthread_ids(SocketId socket_id);
    std::set<HpthreadId> get_hpthread_ids(CoreId core_id);

    /**
     * Reread the node_cpu info.
     */
    void refresh();

private:
    NodeCpu();
    void refresh_node_hpthread();
    void refresh_socket_core_hpthread();
    void refresh_others();
    template <typename K, typename V>
        std::set<K> get_keys(std::map<K, std::set<V> >& data);
    template <typename T>
        std::set<T> get_keys(std::set<T>& data);
    template <typename T>
        int get_key_num(T& data);
    template <typename K, typename V>
        K get_key_by_value(std::map<K, std::set<V> >& data, const V& value);
    template <typename K, typename V>
        std::set<V> get_value_by_key(std::map<K, std::set<V> >& data, const K& key);
    template <typename K, typename V>
        int get_value_num_by_key(std::map<K, std::set<V> >& data, const K& key);

    std::map<NodeId, std::set<SocketId> > node_socket_;
    std::map<NodeId, std::set<CoreId> > node_core_;
    std::map<NodeId, std::set<HpthreadId> > node_hpthread_;
    std::map<SocketId, std::set<CoreId> > socket_core_;
    std::map<SocketId, std::set<HpthreadId> > socket_hpthread_;
    std::map<CoreId, std::set<HpthreadId> > core_hpthread_;
    std::set<HpthreadId> hpthread_;
    std::atomic<bool> inited_;
    std::shared_timed_mutex data_mutex_;


    static constexpr char const * NODE_DIR = "/sys/devices/system/node/";
    static constexpr char const * CPUINFO_FILE = "/proc/cpuinfo";
};

struct NodeId {
    NodeId(): id(-1) { }
    NodeId(int id): id(id) { }
    NodeId(const NodeId& c): id(c.id) { }
    NodeId& operator=(const NodeId& c) { id = c.id; return *this; }
    int id;
};

bool operator==(const NodeId& lc, const NodeId& rc);
bool operator<(const NodeId& lc, const NodeId& rc);
std::ostream &operator<<(std::ostream &os, const NodeId &v);

struct SocketId {
    SocketId(): id(-1) { }
    SocketId(int id): id(id) { }
    SocketId(const SocketId& c): id(c.id) { }
    SocketId& operator=(const SocketId& c) { id = c.id; return *this; }
    int id;
};

bool operator==(const SocketId& lc, const SocketId& rc);
bool operator<(const SocketId& lc, const SocketId& rc);
std::ostream &operator<<(std::ostream &os, const SocketId &v);

struct CoreId {
    CoreId(): socket_id(-1), core_id(-1) { }
    CoreId(int socket_id, int core_id): socket_id(socket_id), core_id(core_id) { }
    CoreId(const CoreId& c): socket_id(c.socket_id), core_id(c.core_id) { }
    CoreId& operator=(const CoreId& c) { socket_id = c.socket_id; core_id = c.core_id; return *this; }
    int socket_id;
    int core_id;
};

bool operator==(const CoreId& lc, const CoreId& rc);
bool operator<(const CoreId& lc, const CoreId& rc);
std::ostream &operator<<(std::ostream &os, const CoreId &v);

struct HpthreadId {
    HpthreadId(): id(-1) { }
    HpthreadId(int id): id(id) { }
    HpthreadId(const HpthreadId& c): id(c.id) { }
    HpthreadId& operator=(const HpthreadId& c) { id = c.id; return *this; }
    int id;
};

bool operator==(const HpthreadId& lc, const HpthreadId& rc);
bool operator<(const HpthreadId& lc, const HpthreadId& rc);
std::ostream &operator<<(std::ostream &os, const HpthreadId &v);

#endif

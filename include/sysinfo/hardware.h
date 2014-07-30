#ifndef _SYSINFO_HARDWARE_H_
#define _SYSINFO_HARDWARE_H_

#include <vector>
#include <string>
#include <set>

using pus_t = const std::set<int>;

struct MicroParam;

/**
 * Collect hardware information.
 * Singlton
 */
class Hardware {
public:
    ~Hardware();
    /**
     * not thread-safe
     */
    static Hardware* get_instance();
    /**
     * Get NUMA node number on this system.
     */
    int get_node_num() const;
    /**
     * Given id of process unit, return the NUMA node id where the
     * process unit located.
     */
    int puid_to_nodeid(int puid) const;
    /**
     * Get system wide number of process units.
     */
    int get_pu_num() const;
    /**
     * Given the id of node, return the number of process units on it.
     */
    int get_pu_num_on_node(int nodeid) const;
    /**
     * Get a set of process unit id on the given NUMA node id.
     */
    pus_t& get_pu_on_node(int nodeid) const;

    std::vector< std::vector<int> > get_node_dist();
    std::vector< std::vector<int> > get_node_dist(MicroParam& param);
    /**
     * Reread the hardware info.
     */
    void refresh();

private:
    Hardware();
    static bool compat_checking();
    static int node_and_digits(const struct dirent *dptr);
    void get_node_dirs(std::vector<std::string>* node_dirs);
    void split(std::string& s, char delim, std::vector<std::string>& ret);

    std::vector<std::set<int> > nodes_;

    static constexpr char const * NODE_DIR = "/sys/devices/system/node/";
    static constexpr int BUF_SIZE = 1024;

};

enum class MicroType : char { RANDOM = 'r', SERIAL = 's' };

struct MicroParam {
    MicroParam() {}
    MicroParam(std::string path, int size_in_mb, MicroType type, int loop):path(path), size_in_mb(size_in_mb), type(type), loop(loop) {}

    std::string path = ".";
    int size_in_mb = 20;
    MicroType type = MicroType::RANDOM;
    int loop = 200;
};

#endif // _SYSINFO_HARDWARE_H_

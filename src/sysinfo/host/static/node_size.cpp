#include <iostream>
#include <inttypes.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include "node_size.h"
using namespace std;

int node_and_digits(const struct dirent *dptr) {
    char *p = (char *)(dptr->d_name);
    if (*p++ != 'n') return 0;
    if (*p++ != 'o') return 0;
    if (*p++ != 'd') return 0;
    if (*p++ != 'e') return 0;
    do {
        if (!isdigit(*p++))
            return 0;
    } while (*p != '\0');
    return 1;
}

int get_node_size_info(node_data_p nodes, size_t nodes_size) {
    char fname[FNAME_SIZE];
    char buf[BIG_BUF_SIZE];
    // Count directory names of the form: /sys/devices/system/node/node<N>
    struct dirent **namelist;
    int num_files = scandir ("/sys/devices/system/node", &namelist, node_and_digits, NULL);
    int num_nodes = num_files;
    if (num_nodes > nodes_size)
    {
        cerr<<"Space of nodes array is insufficient!"<<endl;
    }

    for (int node_ix = 0;  (node_ix < num_nodes);  node_ix++) {
        int node_id = nodes[node_ix].node_id;
        // Get available memory info from node<N>/meminfo file
        snprintf(fname, FNAME_SIZE, "/sys/devices/system/node/node%d/meminfo", node_id);
        int fd = open(fname, O_RDONLY, 0);
        if ((fd >= 0) && (read(fd, buf, BIG_BUF_SIZE) > 0)) {
            uint64_t KB;
            char *p = strstr(buf, "MemTotal:");
            if (p != NULL) {
                p += 9;
            } else {
                cerr<<"Could not get node MemTotal\n";
                return -1;
            }
            while (!isdigit(*p)) { p++; }
            KB = atoi(p);
            while (isdigit(*p)) { p++; }
            nodes[node_ix].MBs_total = KB / KILOBYTE;
            p = strstr(p, "MemFree:");
            if (p != NULL) {
                p += 8;
            } else {
                cerr<<"Could not get node MemFree\n";
                return -1;
            }
            while (!isdigit(*p)) { p++; }
            KB = atoi(p);
            while (isdigit(*p)) { p++; }
            nodes[node_ix].MBs_free = KB / KILOBYTE;
            close(fd);
        } else {
            cerr<<"Could not get node meminfo\n";
            return -1;
        }
    }

    return num_nodes;
}

int main()
{
    node_data nodes[4];
    int n=get_node_size_info(nodes,4);
    for(int i=0;i<n;i++)
    {
        cout<<"node["<<i<<"]:"<<nodes[i].MBs_total<<endl;
    }
    return 0;
}

#include<inttypes.h>
/* const defination */
#define KILOBYTE (1024)
#define MEGABYTE (1024 * 1024)

/* default size defination */
#define FNAME_SIZE 192
#define BUF_SIZE 1024
#define BIG_BUF_SIZE 4096
#define MAX_NODE 1024

typedef struct node_data
{
    uint64_t node_id;
    uint64_t MBs_total;
    uint64_t MBs_free;
} node_data_t, *node_data_p;


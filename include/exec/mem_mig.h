#include "exec/mem_mig.h"

const int MAX_THREAD_NUM = 1024;
const int MAX_BUFFER = 1024;
const int MAX_CPUS = 1024;
const int WAITING_TIME = 1000;

class mem_mig()
{
public:
	static int migrate_memory(int tgid,int from_node,int to_node);
};
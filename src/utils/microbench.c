#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <sched.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define WORKING_SET_SIZE 10000
#define SHARED_SIZE 0
#define LOOP_NUM 10000
#define THREAD_NUM 1
#define SEC_PER_USEC 1000000
#define SEC_PER_MSEC 1000
#define MSEC_PER_USEC 1000
#define MAX_CPUS 256
#define MASK 0xd0000001u
#define STREAM_TYPE  1
#define RANDOM_TYPE  0
#define BUFLEN 128

#define lrand(lfsr) (lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & MASK))
#define rnum(lfsr) (lrand(lfsr) % working_set_size)

typedef struct thread_arg
{
	uint64_t * private_data;
	uint64_t * shared_data;
	uint64_t * stream_data;
    uint64_t * dump;

    int thread_id;
	int binding_cpu_id;
}thread_arg, * thread_arg_p;

typedef struct bench_time
{
	uint64_t private_data_time;
	uint64_t shared_data_time;
	uint64_t total_time;
}bench_time, * bench_time_p;

void * thread_main (void * arg);
uint64_t get_timeval_in_usec(struct timeval start_time,struct timeval end_time);
char * parse_time (char* buf, int len, uint64_t usec);
uint64_t parse_bytes(char * tar);
uint64_t parse_types(char * tar);
void show_error_and_help(char * msg);
void show_help();

//uint64_t lfsr=1;

uint64_t working_set_size = WORKING_SET_SIZE;
uint64_t shared_size = SHARED_SIZE;
uint64_t loop_num = LOOP_NUM;
uint64_t thread_num = THREAD_NUM;
uint64_t total_running_time = 0;
uint64_t workload_type = 0;
uint64_t cpu_bindings[MAX_CPUS];

struct timeval bench_start, bench_end;
bench_time_p run_result;
pthread_mutex_t total_running_time_mutex;

//uint64_t dump[100];

//IEEE MICRO'12 
//Bubble-up: Increasing Utilization in Modern Warehouse Scale Computers via Sensible Co-locations
/*
void single_static_assignment(uint64_t * dump, uint64_t * data_chunk, uint64_t thread_id)
{
	uint64_t i=0;
    struct timeval timeseed;
    gettimeofday(&timeseed, NULL);
	uint64_t lfsr = timeseed.tv_usec % 11 +1;
	while(i < loop_num){
		dump[0]+=data_chunk[rnum(lfsr)]++;
		dump[1]+=data_chunk[rnum(lfsr)]++;
		dump[2]+=data_chunk[rnum(lfsr)]++;
		dump[3]+=data_chunk[rnum(lfsr)]++;
		dump[4]+=data_chunk[rnum(lfsr)]++;
		dump[5]+=data_chunk[rnum(lfsr)]++;
		dump[6]+=data_chunk[rnum(lfsr)]++;
		dump[7]+=data_chunk[rnum(lfsr)]++;
		dump[8]+=data_chunk[rnum(lfsr)]++;
		dump[9]+=data_chunk[rnum(lfsr)]++;
		dump[10]+=data_chunk[rnum(lfsr)]++;
		dump[11]+=data_chunk[rnum(lfsr)]++;
		dump[12]+=data_chunk[rnum(lfsr)]++;
		dump[13]+=data_chunk[rnum(lfsr)]++;
		dump[14]+=data_chunk[rnum(lfsr)]++;
		dump[15]+=data_chunk[rnum(lfsr)]++;
		dump[16]+=data_chunk[rnum(lfsr)]++;
		dump[17]+=data_chunk[rnum(lfsr)]++;
		dump[18]+=data_chunk[rnum(lfsr)]++;
		dump[19]+=data_chunk[rnum(lfsr)]++;
		dump[20]+=data_chunk[rnum(lfsr)]++;
		dump[21]+=data_chunk[rnum(lfsr)]++;
		dump[22]+=data_chunk[rnum(lfsr)]++;
		dump[23]+=data_chunk[rnum(lfsr)]++;
		dump[24]+=data_chunk[rnum(lfsr)]++;
		dump[25]+=data_chunk[rnum(lfsr)]++;
		dump[26]+=data_chunk[rnum(lfsr)]++;
		dump[27]+=data_chunk[rnum(lfsr)]++;
		dump[28]+=data_chunk[rnum(lfsr)]++;
		dump[29]+=data_chunk[rnum(lfsr)]++;
		dump[30]+=data_chunk[rnum(lfsr)]++;
		dump[31]+=data_chunk[rnum(lfsr)]++;
		dump[32]+=data_chunk[rnum(lfsr)]++;
		dump[33]+=data_chunk[rnum(lfsr)]++;
		dump[34]+=data_chunk[rnum(lfsr)]++;
		dump[35]+=data_chunk[rnum(lfsr)]++;
		dump[36]+=data_chunk[rnum(lfsr)]++;
		dump[37]+=data_chunk[rnum(lfsr)]++;
		dump[38]+=data_chunk[rnum(lfsr)]++;
		dump[39]+=data_chunk[rnum(lfsr)]++;
		dump[40]+=data_chunk[rnum(lfsr)]++;
		dump[41]+=data_chunk[rnum(lfsr)]++;
		dump[42]+=data_chunk[rnum(lfsr)]++;
		dump[43]+=data_chunk[rnum(lfsr)]++;
		dump[44]+=data_chunk[rnum(lfsr)]++;
		dump[45]+=data_chunk[rnum(lfsr)]++;
		dump[46]+=data_chunk[rnum(lfsr)]++;
		dump[47]+=data_chunk[rnum(lfsr)]++;
		dump[48]+=data_chunk[rnum(lfsr)]++;
		dump[49]+=data_chunk[rnum(lfsr)]++;
		dump[50]+=data_chunk[rnum(lfsr)]++;
		dump[51]+=data_chunk[rnum(lfsr)]++;
		dump[52]+=data_chunk[rnum(lfsr)]++;
		dump[53]+=data_chunk[rnum(lfsr)]++;
		dump[54]+=data_chunk[rnum(lfsr)]++;
		dump[55]+=data_chunk[rnum(lfsr)]++;
		dump[56]+=data_chunk[rnum(lfsr)]++;
		dump[57]+=data_chunk[rnum(lfsr)]++;
		dump[58]+=data_chunk[rnum(lfsr)]++;
		dump[59]+=data_chunk[rnum(lfsr)]++;
		dump[60]+=data_chunk[rnum(lfsr)]++;
		dump[61]+=data_chunk[rnum(lfsr)]++;
		dump[62]+=data_chunk[rnum(lfsr)]++;
		dump[63]+=data_chunk[rnum(lfsr)]++;
		dump[64]+=data_chunk[rnum(lfsr)]++;
		dump[65]+=data_chunk[rnum(lfsr)]++;
		dump[66]+=data_chunk[rnum(lfsr)]++;
		dump[67]+=data_chunk[rnum(lfsr)]++;
		dump[68]+=data_chunk[rnum(lfsr)]++;
		dump[69]+=data_chunk[rnum(lfsr)]++;
		dump[70]+=data_chunk[rnum(lfsr)]++;
		dump[71]+=data_chunk[rnum(lfsr)]++;
		dump[72]+=data_chunk[rnum(lfsr)]++;
		dump[73]+=data_chunk[rnum(lfsr)]++;
		dump[74]+=data_chunk[rnum(lfsr)]++;
		dump[75]+=data_chunk[rnum(lfsr)]++;
		dump[76]+=data_chunk[rnum(lfsr)]++;
		dump[77]+=data_chunk[rnum(lfsr)]++;
		dump[78]+=data_chunk[rnum(lfsr)]++;
		dump[79]+=data_chunk[rnum(lfsr)]++;
		dump[80]+=data_chunk[rnum(lfsr)]++;
		dump[81]+=data_chunk[rnum(lfsr)]++;
		dump[82]+=data_chunk[rnum(lfsr)]++;
		dump[83]+=data_chunk[rnum(lfsr)]++;
		dump[84]+=data_chunk[rnum(lfsr)]++;
		dump[85]+=data_chunk[rnum(lfsr)]++;
		dump[86]+=data_chunk[rnum(lfsr)]++;
		dump[87]+=data_chunk[rnum(lfsr)]++;
		dump[88]+=data_chunk[rnum(lfsr)]++;
		dump[89]+=data_chunk[rnum(lfsr)]++;
		dump[90]+=data_chunk[rnum(lfsr)]++;
		dump[91]+=data_chunk[rnum(lfsr)]++;
		dump[92]+=data_chunk[rnum(lfsr)]++;
		dump[93]+=data_chunk[rnum(lfsr)]++;
		dump[94]+=data_chunk[rnum(lfsr)]++;
		dump[95]+=data_chunk[rnum(lfsr)]++;
		dump[96]+=data_chunk[rnum(lfsr)]++;
		dump[97]+=data_chunk[rnum(lfsr)]++;
		dump[98]+=data_chunk[rnum(lfsr)]++;
		dump[99]+=data_chunk[rnum(lfsr)]++;

//        printf("%ld,%ld,%ld\n",rnum(lfsr),rnum(lfsr),rnum(lfsr));
		i++;
	}
}
*/

void single_static_assignment(uint64_t * dump, uint64_t * data_chunk, uint64_t thread_id)
{
	uint64_t i=0,j=0,k=0,s1,s2,s3,len;
    struct timeval timeseed;
    gettimeofday(&timeseed, NULL);
	uint64_t lfsr = timeseed.tv_usec % 11 +1;
	uint64_t *mid = data_chunk + (working_set_size/2);
    len = working_set_size/800;
    while(i < loop_num){
		for(j=0;j<len;j++){
        dump[0]+=data_chunk[rnum(lfsr)]++;
		dump[1]+=data_chunk[rnum(lfsr)]++;
		dump[2]+=data_chunk[rnum(lfsr)]++;
		dump[3]+=data_chunk[rnum(lfsr)]++;
		dump[4]+=data_chunk[rnum(lfsr)]++;
		dump[5]+=data_chunk[rnum(lfsr)]++;
		dump[6]+=data_chunk[rnum(lfsr)]++;
		dump[7]+=data_chunk[rnum(lfsr)]++;
		dump[8]+=data_chunk[rnum(lfsr)]++;
		dump[9]+=data_chunk[rnum(lfsr)]++;
		dump[10]+=data_chunk[rnum(lfsr)]++;
		dump[11]+=data_chunk[rnum(lfsr)]++;
		dump[12]+=data_chunk[rnum(lfsr)]++;
		dump[13]+=data_chunk[rnum(lfsr)]++;
		dump[14]+=data_chunk[rnum(lfsr)]++;
		dump[15]+=data_chunk[rnum(lfsr)]++;
		dump[16]+=data_chunk[rnum(lfsr)]++;
		dump[17]+=data_chunk[rnum(lfsr)]++;
		dump[18]+=data_chunk[rnum(lfsr)]++;
		dump[19]+=data_chunk[rnum(lfsr)]++;
		dump[20]+=data_chunk[rnum(lfsr)]++;
		dump[21]+=data_chunk[rnum(lfsr)]++;
		dump[22]+=data_chunk[rnum(lfsr)]++;
		dump[23]+=data_chunk[rnum(lfsr)]++;
		dump[24]+=data_chunk[rnum(lfsr)]++;
		dump[25]+=data_chunk[rnum(lfsr)]++;
		dump[26]+=data_chunk[rnum(lfsr)]++;
		dump[27]+=data_chunk[rnum(lfsr)]++;
		dump[28]+=data_chunk[rnum(lfsr)]++;
		dump[29]+=data_chunk[rnum(lfsr)]++;
		dump[30]+=data_chunk[rnum(lfsr)]++;
		dump[31]+=data_chunk[rnum(lfsr)]++;
		dump[32]+=data_chunk[rnum(lfsr)]++;
		dump[33]+=data_chunk[rnum(lfsr)]++;
		dump[34]+=data_chunk[rnum(lfsr)]++;
		dump[35]+=data_chunk[rnum(lfsr)]++;
		dump[36]+=data_chunk[rnum(lfsr)]++;
		dump[37]+=data_chunk[rnum(lfsr)]++;
		dump[38]+=data_chunk[rnum(lfsr)]++;
		dump[39]+=data_chunk[rnum(lfsr)]++;
		dump[40]+=data_chunk[rnum(lfsr)]++;
		dump[41]+=data_chunk[rnum(lfsr)]++;
		dump[42]+=data_chunk[rnum(lfsr)]++;
		dump[43]+=data_chunk[rnum(lfsr)]++;
		dump[44]+=data_chunk[rnum(lfsr)]++;
		dump[45]+=data_chunk[rnum(lfsr)]++;
		dump[46]+=data_chunk[rnum(lfsr)]++;
		dump[47]+=data_chunk[rnum(lfsr)]++;
		dump[48]+=data_chunk[rnum(lfsr)]++;
		dump[49]+=data_chunk[rnum(lfsr)]++;
		dump[50]+=data_chunk[rnum(lfsr)]++;
		dump[51]+=data_chunk[rnum(lfsr)]++;
		dump[52]+=data_chunk[rnum(lfsr)]++;
		dump[53]+=data_chunk[rnum(lfsr)]++;
		dump[54]+=data_chunk[rnum(lfsr)]++;
		dump[55]+=data_chunk[rnum(lfsr)]++;
		dump[56]+=data_chunk[rnum(lfsr)]++;
		dump[57]+=data_chunk[rnum(lfsr)]++;
		dump[58]+=data_chunk[rnum(lfsr)]++;
		dump[59]+=data_chunk[rnum(lfsr)]++;
		dump[60]+=data_chunk[rnum(lfsr)]++;
		dump[61]+=data_chunk[rnum(lfsr)]++;
		dump[62]+=data_chunk[rnum(lfsr)]++;
		dump[63]+=data_chunk[rnum(lfsr)]++;
		dump[64]+=data_chunk[rnum(lfsr)]++;
		dump[65]+=data_chunk[rnum(lfsr)]++;
		dump[66]+=data_chunk[rnum(lfsr)]++;
		dump[67]+=data_chunk[rnum(lfsr)]++;
		dump[68]+=data_chunk[rnum(lfsr)]++;
		dump[69]+=data_chunk[rnum(lfsr)]++;
		dump[70]+=data_chunk[rnum(lfsr)]++;
		dump[71]+=data_chunk[rnum(lfsr)]++;
		dump[72]+=data_chunk[rnum(lfsr)]++;
		dump[73]+=data_chunk[rnum(lfsr)]++;
		dump[74]+=data_chunk[rnum(lfsr)]++;
		dump[75]+=data_chunk[rnum(lfsr)]++;
		dump[76]+=data_chunk[rnum(lfsr)]++;
		dump[77]+=data_chunk[rnum(lfsr)]++;
		dump[78]+=data_chunk[rnum(lfsr)]++;
		dump[79]+=data_chunk[rnum(lfsr)]++;
		dump[80]+=data_chunk[rnum(lfsr)]++;
		dump[81]+=data_chunk[rnum(lfsr)]++;
		dump[82]+=data_chunk[rnum(lfsr)]++;
		dump[83]+=data_chunk[rnum(lfsr)]++;
		dump[84]+=data_chunk[rnum(lfsr)]++;
		dump[85]+=data_chunk[rnum(lfsr)]++;
		dump[86]+=data_chunk[rnum(lfsr)]++;
		dump[87]+=data_chunk[rnum(lfsr)]++;
		dump[88]+=data_chunk[rnum(lfsr)]++;
		dump[89]+=data_chunk[rnum(lfsr)]++;
		dump[90]+=data_chunk[rnum(lfsr)]++;
		dump[91]+=data_chunk[rnum(lfsr)]++;
		dump[92]+=data_chunk[rnum(lfsr)]++;
		dump[93]+=data_chunk[rnum(lfsr)]++;
		dump[94]+=data_chunk[rnum(lfsr)]++;
		dump[95]+=data_chunk[rnum(lfsr)]++;
		dump[96]+=data_chunk[rnum(lfsr)]++;
		dump[97]+=data_chunk[rnum(lfsr)]++;
		dump[98]+=data_chunk[rnum(lfsr)]++;
		dump[99]+=data_chunk[rnum(lfsr)]++;
        }
/*        s1=rnum(lfsr); s2=rnum(lfsr); s3=rnum(lfsr);
        if((s1+5000) > working_set_size) s1=s1-5005;
        if((s2+5000) > working_set_size) s2=s2-5005;
        if((s3+5000) > working_set_size) s3=s3-5005;
        for(j=0;j<5000;j++){
            data_chunk[j+s1] = data_chunk[j+s2] + data_chunk[j+s3];
        }
*/
        for(j=0;j<working_set_size/2;j++)
            data_chunk[j] = mid[j] + 1;
        for(j=0;j<working_set_size/2;j++)
            mid[j] = data_chunk[j] + 1;
/*        
        for(j=0;j<2000;j++){
            data_chunk[k] = mid[k]+1;
            k++;
            if( (k+k) >= working_set_size)
                k = 0;
        }
        
        for(j=0;j<2000;j++){
            mid[k] = data_chunk[k]+1;
            k++;
            if( (k+k) >= working_set_size)
                k = 0;
        }
*/
        i++;
	}
}

/*
void stream_access_bandwidth(double * bw_data, uint64_t bw_stream_size)
{
	uint64_t i, j=0;
	while(j < loop_num){
		double scalar = 1.3;
		double *mid = bw_data + (bw_stream_size / 2);

		for(i=0; i < bw_stream_size/2; i++){
			bw_data[i] = scalar * mid[i];
		}

		for(i=0; i < bw_stream_size/2; i++){
			mid[i] = scalar * bw_data[i];
		}
		j++;
	}
}
*/

void stream_access_bandwidth(uint64_t* bw_data, uint64_t bw_stream_size)
{
	uint64_t i, j=0;
	while(j < loop_num){
//		double scalar = 1.3;
		uint64_t *mid = bw_data + (bw_stream_size / 2);

		for(i=0; i < bw_stream_size/2; i++){
			bw_data[i] = mid[i] + 1;
		}

		for(i=0; i < bw_stream_size/2; i++){
			mid[i] = bw_data[i] + 1;
		}
		j++;
	}
}
int main (int argc, char * argv[])
{
	int i=0;

	
    if (argc > 1)
    {
        for (i = 1; i < argc; i ++)
        {
            if (strcmp (argv[i], "--working_set_size") == 0 || strcmp(argv[i], "-w") == 0)
            {
                if (i + 1 == argc || (working_set_size = atoi (argv[++i])) < 0)
                {
                    show_error_and_help ("Set working_set_size parameters error!\n\n");
                }

                continue;
            }
            if (strcmp (argv[i], "--working_set_bytes") == 0 || strcmp(argv[i], "-wb") == 0)
            {
                int64_t working_set_bytes;
                if (i + 1 == argc || (working_set_bytes = parse_bytes (argv[++i])) < 0)
                {
                    show_error_and_help ("Set working_set_bytes parameters error!\n\n");
                }

                working_set_size = (uint64_t)(working_set_bytes / sizeof (uint64_t));
                continue;
            }
            else if (strcmp (argv[i], "--shared_size") == 0 || strcmp(argv[i], "-s") == 0)
            {
                if (i + 1 == argc || (shared_size = atoi (argv[++i])) < 0)
                {
                    show_error_and_help ("Set shared_size parameters error!\n\n");
                }

                continue;
            }
            else if (strcmp (argv[i], "--shared_bytes") == 0 || strcmp(argv[i], "-sb") == 0)
            {
                int64_t shared_bytes;
                if (i + 1 == argc || (shared_bytes = parse_bytes (argv[++i])) < 0)
                {
                    show_error_and_help ("Set shared_bytes parameters error!\n\n");
                }

                shared_size = (uint64_t)(shared_bytes / sizeof (uint64_t));
                continue;
            }
			else if (strcmp (argv[i], "--workload_type") == 0 || strcmp(argv[i], "-wt") == 0)
            {
                if (i + 1 == argc || (workload_type = parse_types (argv[++i])) < 0)
                {
                    show_error_and_help ("Set shared_bytes parameters error!\n\n");
                }

                continue;
            }
            else if (strcmp (argv[i], "--loop") == 0 || strcmp(argv[i], "-l") == 0)
            {
                if (i + 1 == argc || (loop_num= atoi (argv[++i])) <= 0)
                {
                    show_error_and_help ("Set loop_num parameters error!\n\n");
                }

                continue;
            }
            else if (strcmp (argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0)
            {
                if (i + 1 == argc || (thread_num = atoi (argv[++i])) <= 0)
                {
                    show_error_and_help ("Set thread_num parameters error!\n\n");
                }

                continue;
            }
            else if (strcmp (argv[i], "--binding") == 0 || strcmp(argv[i], "-b") == 0)
            {
                uint64_t cpuid, bind_num;
                if (i + 2 >= argc || (cpuid = atoi (argv[++i])) < 0 || (bind_num = atoi (argv[++i])) <= 0)
                {
                    show_error_and_help ("Set binding parameters error!\n\n");
                }

                cpu_bindings[cpuid] = bind_num;
            }
            else if (strcmp (argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
            {
                show_help();
                continue;
            }
        }
    }
    else
    {
        show_help();
    }

//	printf ("working_set_size = %-10d = %d bytes\n", working_set_size, working_set_size * 64);
//    printf ("shared_size      = %-10d = %d bytes\n", shared_size, shared_size * 8);
//    printf ("loop             = %d\n", loop_num);
//    printf ("threads_number   = %d\n\n", thread_num);

	//memset(dump, 0, 100 * sizeof(uint64_t));
	if (working_set_size <= 0)
        return 0;

	uint64_t * shared_data = NULL;
	if (shared_size > 0){
		shared_data = (uint64_t *)malloc(shared_size * sizeof(uint64_t));
		if(shared_data == NULL){
			printf("Failed to malloc shared data!\n");
			exit(1);
		}
		memset(shared_data, 0, shared_size * sizeof(uint64_t));
	}

	pthread_t * threads = malloc(thread_num * sizeof(pthread_t));
	if (threads == NULL){
		printf("Failed to malloc threads struct!\n");
		exit(1);
	}

	if (pthread_mutex_init (&total_running_time_mutex, NULL) != 0){
		printf ("Failed to init total_running_time_mutex!\n");
        exit(1);
	}

	thread_arg_p t_args = (thread_arg_p)malloc(thread_num * sizeof(thread_arg));
    if (t_args == NULL)
    {
        printf ("Fail to malloc thread arg\n");
        exit(1);
    }

	if ((run_result=(bench_time_p)malloc(thread_num * sizeof(bench_time))) == NULL)
    {
        printf ("Failed to malloc thread bench result!\n");
        exit(1);
    }
    memset (run_result, 0, thread_num * sizeof (bench_time));

	uint64_t find_cpu_bindings = 1;

	for (i=0; i < thread_num; i++){
		(t_args + i) -> shared_data = shared_data;
		(t_args + i) -> thread_id = i;
		(t_args + i) -> binding_cpu_id = -1;
/*
		(t_args + i) -> private_data = (uint64_t *)malloc(working_set_size * sizeof(uint64_t));
		if((t_args + i) -> private_data == NULL){
			printf("Failed to malloc private data for thread %d\n.", i);
			exit(1);
		}
*/
		if (find_cpu_bindings){
			int j;
			for(j = 0; j < MAX_CPUS; j++){
				if (cpu_bindings[j] > 0){
					(t_args + i) -> binding_cpu_id = j;
					cpu_bindings[j]--;
					break;
				}
			}

			if(j >= MAX_CPUS){
				find_cpu_bindings = 0;
			}
		}

		if (pthread_create(threads+i, NULL, thread_main, t_args+i) != 0){
			printf("Failed to create thread %d\n", i);
			exit(1);
		}
	}

	gettimeofday (&bench_start, NULL);

	for (i=0; i<thread_num; i++){
		pthread_join(threads[i], NULL);
	}

	gettimeofday (&bench_end, NULL);

	uint64_t time_used = get_timeval_in_usec(bench_start, bench_end);

	char buf[BUFLEN];
   // printf ("\nThread ID\tRunning Time\n");
    for (i = 0; i < thread_num; i ++)
    {
       // printf ("Thread %i :\t%s ", i, parse_time(buf, BUFLEN, run_result[i].total_time));
        printf("%s ", parse_time(buf, BUFLEN, run_result[i].total_time));
        if ((t_args+i)->binding_cpu_id >= 0)
        {
            printf ("running at cpu %d", (t_args+i)->binding_cpu_id);
        }

        printf ("\n");
    }

//    printf ("\nProgram running time        = %s\n", parse_time(buf, BUFLEN, time_used));
//    printf ("Thread totally time sum     = %s\n", parse_time(buf, BUFLEN, total_running_time));
//    printf ("Thread average running time = %s\n", parse_time(buf, BUFLEN, total_running_time / thread_num));

    free (threads);
    free (shared_data);
	for(i=0; i<thread_num; i++){
		free ((t_args + i) -> private_data);
	}
    free (t_args);
    free (run_result);
	
    return 0;
}

void * thread_main (void * arg)
{
	int binding_cpu = ((thread_arg_p)arg) -> binding_cpu_id;
	if (binding_cpu >=0){
		cpu_set_t mask, get;
		CPU_ZERO (&mask);
		CPU_SET (binding_cpu, &mask);
		if (sched_setaffinity(0, sizeof(mask), &mask) < 0){
			printf("Failed to sched_setaffinity of thread %d\n", ((thread_arg_p)arg) -> thread_id);
			return;
		}
	}

	uint64_t * th_shared_data = ((thread_arg_p)arg)->shared_data;
	int thread_id = ((thread_arg_p)arg)->thread_id;

	struct timeval th_start_time, th_end_time;
	int i, n, j;
	uint64_t tmp;

	//Initialize private_data set
	((thread_arg_p)arg) -> private_data = (uint64_t *)malloc(working_set_size * sizeof(uint64_t));
	if(((thread_arg_p)arg) -> private_data == NULL){
		printf("Failed to malloc private data for thread %d\n.", thread_id);
		return ;
	}
	memset(((thread_arg_p)arg) -> private_data, 0, working_set_size * sizeof(uint64_t));

	((thread_arg_p)arg) -> dump = (uint64_t *)malloc(100 * sizeof(uint64_t));
	if(((thread_arg_p)arg) -> dump == NULL){
		printf("Failed to malloc dump data for thread %d\n.", thread_id);
		return ;
	}
	memset(((thread_arg_p)arg) -> dump, 0, 100 * sizeof(uint64_t));
	
    if(workload_type == STREAM_TYPE){
    //Initialize stream_data set
	    ((thread_arg_p)arg) -> stream_data = (uint64_t *)malloc(working_set_size * sizeof(uint64_t));
	    if(((thread_arg_p)arg) -> stream_data == NULL){
		    printf("Failed to malloc stream data for thread %d\n.", thread_id);
		    return ;
	    }
	    memset(((thread_arg_p)arg) -> stream_data, 0, working_set_size * sizeof(uint64_t));
    }
	gettimeofday (&th_start_time, NULL);

	switch(workload_type){
		case RANDOM_TYPE:
			single_static_assignment(((thread_arg_p)arg) -> dump, ((thread_arg_p)arg) -> private_data, ((thread_arg_p)arg) -> thread_id);
			break;
		case STREAM_TYPE:
			stream_access_bandwidth(((thread_arg_p)arg) -> stream_data, working_set_size);
			break;
		default:
			printf("workload type is out of range!\n");
			break;
	}

	gettimeofday (&th_end_time, NULL);

	uint64_t th_time_used = get_timeval_in_usec(th_start_time,th_end_time);
	run_result[thread_id].total_time = th_time_used;

	__sync_add_and_fetch (&total_running_time, th_time_used);

//	printf("At Time %lld s: Thread %d was finished!\n", th_end_time.tv_sec, thread_id);

	return NULL;
}

uint64_t parse_bytes (char * tar)
{
    if (tar == NULL || strlen (tar) == 0 || tar[0] < '0' || tar[0] > '9')
    {
        return 0;
    }

    uint64_t number = atoll (tar);
    if (number == 0)
    {
        return 0;
    }

    char unit = tar [strlen(tar) - 1];
    if (tolower(unit) == 'm')
    {
        return number * 1024 * 1024;
    }
    else if (tolower(unit) == 'k')
    {
        return number * 1024;
    }
    else if (tolower(unit) == 'b')
    {
        return number;
    }
    else
    {
        return 0;
    }
}

uint64_t parse_types (char * tar)
{
	if (tar[0] == 'r'){
		return RANDOM_TYPE;
	}
	else if (tar[0] == 's'){
		return STREAM_TYPE;
	}
	else
		return -1;
}

uint64_t get_timeval_in_usec (struct timeval start_time, struct timeval end_time)
{
    return SEC_PER_USEC * (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec);
}

char * parse_time (char * buf, int len, uint64_t usec)
{
   // snprintf (buf, len, "%lld s %lld ms %lld us", usec / SEC_PER_USEC, usec % SEC_PER_USEC / MSEC_PER_USEC, usec % MSEC_PER_USEC); 
    snprintf (buf, len, "%lf", usec*1.0 / SEC_PER_USEC);
    return buf;
}

void show_error_and_help (char * msg)
{
    printf (msg);
    show_help();
}

void show_help ()
{
    printf ("Pressure Test [--help] [--working_set_size N] [--shared_size N] [--loop N]\n\n");
    printf ("-w   --working_set_size   to specify the size of private working set using random\n");
    printf ("                          single static assignment, each element is 64 bit and the total\n");
    printf ("                          size is woking_set_size * 64 bit\n\n");
    printf ("-wb  --working_set_bytes  to specify the size of private working set\n");
    printf ("                          by bytes in unit m, k, b ex. 64k for 64k working set\n\n");
    printf ("-s   --shared_size        to specify the shared space by threads visiting an uint64_t\n");
    printf ("                          array and the tatol size is shared_size * 8 bytes\n\n");
    printf ("-sb  --shared_bytes       to specify the shared space by threads by bytes in unit\n");
    printf ("                          k, m, b ex. -sb 8m for 8mb shared space\n\n");
    printf ("-wt  --workload_type      wokload type: r indicates single_static_assignment,\n");
    printf ("                                        s indicates stream_access_bandwidth\n");
    printf ("-l   --loop_num           times to visit working set and shared memory space\n\n");
    printf ("-t   --threads            running threads of micro benchmark\n\n");
    printf ("-b   --binding            binding cpu affinity with two parameters, the first one to\n");
    printf ("                          specify the cpuid to bind and the second is the number of threads\n");
    printf ("                          binding on this cpu, ex. --bind 0 2 for 2 threads are binding to\n");
    printf ("                          cpu 0, supporting more than one binding rules\n\n");
    printf ("-h   --help               to show this help\n\n");
    printf ("Version 1.0 by Yuxia @ 2013.07.31\n");
    printf ("Thanks!.\n");
    exit (0);
}


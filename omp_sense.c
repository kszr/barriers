#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

static int sense = 1;
static int num_procs;
static int num_barriers;
static int count;

void central_barrier (int* locksense){
	*locksense = ! *locksense;
	
	#pragma omp critical
		count -= 1;
	
	if (count == 0) {
		count = num_procs;
		sense = *locksense;
	} else
		while (sense != *locksense);
}

int main(int argc, char* argv[]) {
	if (argc == 3) {
		num_procs = atoi (argv[1]);
		count = num_procs;
		num_barriers = atoi (argv[2]);
	} else {
		printf("Enter the total number of processors and how many times you want to simulate the barrier.\n");
		exit(-1);
		}
	
	/*Serial Code started*/
	printf("Serial Section started.\n");
	omp_set_num_threads(num_procs);
	double T1, T2;
	
	/*Parallel Code started*/
	#pragma omp parallel shared (sense, count)
	{
		int thread_id = omp_get_thread_num();
		int locksense = 1;
		long i;
		
		for(i=0; i<num_barriers; i++) {
			printf("Thread %d in parallel section.\n", thread_id);
			T1 = omp_get_wtime();
			central_barrier(&locksense);
			T2 = omp_get_wtime();
			printf("Thread %d out from parallel section.\n", thread_id);
		}
		printf("Thread %d spent time= %f \n", thread_id, T2-T1);
	}
	
	printf("Control back in to Serial Section.\n");
	return 0;	
}

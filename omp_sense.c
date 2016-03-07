/**
 * @author: Abhishek Patil
 */
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <sys/time.h>

static int sense = 1;
static int num_procs;
static int num_barriers;
static int count;

void central_barrier (int* local_sense){
	*local_sense = ! *local_sense;
	
	#pragma omp critical
		count -= 1;
	
	if (count == 0) {
		count = num_procs;
		sense = *local_sense;
	} else
		while (sense != *local_sense);
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
	
	omp_set_num_threads(num_procs);
	//struct timeval  T1, T2;
	double T1, T2;
	/*Parallel Code started*/
	#pragma omp parallel shared (sense, count)
	{
		int thread_id = omp_get_thread_num();
		int local_sense = 1;
		long i;
		T1 = omp_get_wtime();
		//gettimeofday(&T1, NULL);
		for(i=0; i<num_barriers; i++) {
			fprintf(stderr, "%lu\n", i);
			central_barrier(&local_sense);

		}

		T2 = omp_get_wtime();
		//gettimeofday(&T2, NULL);
		//printf("Thread %d spent time= %f \n", thread_id, elapsedTime(T1, T2));
		printf("took %fs to run with %d processors\n", (T2-T1), num_procs);
	}

	return 0;	
}

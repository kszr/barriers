/**
 * @author: Abhishek Patil
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include "omp_dissemination.h"

/*
Implementing a dissemination barrier with only local spinning from MCS paper
*/

static int num_procs;
static int num_barriers;
static int num_rounds;

void initialize_barrier(flags *allnodes){
	int i, k;
	for(i=0; i<num_procs; i++)
		for(k=0; k<num_rounds; k++) {
			allnodes[i].myflags[0][k] = 0;
			allnodes[i].myflags[1][k] = 0;
		}

}

void dissemination_barrier (flags *localflags, int *parity, int *sense){
	int k;
	for (k=0; k<num_rounds; k++) {
		#pragma omp critical
			*localflags->partnerflags[*parity][k] = *sense;
		while(localflags->myflags[*parity][k] != *sense) {}
	}
	
	if(*parity == 1)
		*sense = !*sense;
	*parity = 1 - *parity;

}

int main(int argc, char* argv[]) {
	if (argc == 3) {
		num_procs = atoi (argv[1]);
		num_barriers = atoi (argv[2]);
	} else {
		printf("Enter the total number of processors and how many times you want to simulate the barrier.\n");
		exit(-1);
		}
	
	/*Serial Code started*/
	// printf("Serial Section started.\n");
	flags allnodes[num_procs];
	num_rounds = ceil (log(num_procs)/log(2));
	omp_set_num_threads(num_procs);
	double T1, T2;
	
	/*Parallel Code started*/
	#pragma omp parallel shared (allnodes, num_rounds)
	{
		int thread_id = omp_get_thread_num();
		long i;
		int j, r, k;
		flags *localflags = &allnodes[thread_id];
		int parity = 0;
		int sense = 1;
		#pragma omp single nowait
			initialize_barrier(allnodes);
		T1 = omp_get_wtime();
		for(i=0; i<num_barriers; i++) {
			// printf("Thread %d in parallel section.\n", thread_id);
			#pragma omp critical
				for (j=0; j<num_procs; j++)
					for (k=0; k<num_rounds; k++) {
						if (j == ((thread_id + (int)(pow(2,k))) % num_procs)) {
							for(r=0; r<2; r++)
								allnodes[thread_id].partnerflags[r][k] = &allnodes[j].myflags[r][k];
						}
					}			
			//T1 = omp_get_wtime();
			dissemination_barrier(localflags, &parity, &sense);
			//T2 = omp_get_wtime();
			// printf("Thread %d out from parallel section.\n", thread_id);
		}
		T2 = omp_get_wtime();
		printf("Thread %d spent time= %f \n", thread_id, T2-T1);
	}
	
	// printf("Control back in to Serial Section.\n");
	return 0;	
}

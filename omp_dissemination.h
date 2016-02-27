#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

/*
Implementing a dissemination barrier with only local spinning from MCS paper
*/

#define MAX_PROCS 1000

typedef struct flags {
	int myflags[2][MAX_PROCS];
	int *partnerflags[2][MAX_PROCS];
} flags;

void initialize_barrier(flags *allnodes);
void dissemination_barrier(flags *localflags, int *parity, int *sense);

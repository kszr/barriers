/**
 * An implementation of a dissemination barrier, following the algorithm 
 * in the Mellor-Crummey and Scott paper.
 *
 * @author: Abhishek Chatterjee [achatterjee32]
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dissemination.h"


int main(int argc, char *argv[]) {
    int id;
    int ierr;
    double wtime;
    int num_iters = 1;

    // Initiaize MPI
    ierr = MPI_Init(&argc, &argv);

    // Get number of processores.
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Get individual processor id.
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &id);

    tree_size = exp2(log2(num_procs)+ 1)-1; // Size of a complete binary tree is 2^(h+1)-1; size of level h is 2^h.
    
    // Initialize the processor struct for this processor.
    processor_t processor;
    processor.id = id;
    processor.round = 0;
    processor.has_sent = 0;
    processor.locksense = 0;

    if(id == 0)
        wtime = MPI_Wtime();

    if(argc >=2) {
        if(strcmp(argv[argc-2], "-i") == 0) {
            num_iters = atoi(argv[argc-1]);
        } else {
            fprintf(stderr, "Usage: ./tourney_tree -i [num_iters]");
            return 1;      
        }
        if(num_iters < 0) {
            fprintf(stderr, "Usage: ./tourney_tree -i [num_iters]");
            return 1;
        }
    }
    
    printf("argc = %d; Num iters = %d\n", argc, num_iters);
    int i;
    for(i=0; i<num_iters; i++)   
        join_tournament(&processor); 

    if(id == 0) {
        wtime = MPI_Wtime() - wtime;
        printf("Barrier took %fs to run with %d processors.\n", wtime, num_procs);
    }

    MPI_Finalize();
    return 0;
}


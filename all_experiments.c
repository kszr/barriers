#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi_mcs.h"
#include "mpi_tournament.h"

int num_procs;

int mcs(int argc, char *argv[]) {
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
   
    // Initialize the treenode for this processor.
    treenode_t treenode;
    treenode.processid = id;
    treenode.parentsense = 0;

    int i;
    for(i=0; i<4; i++) {
        treenode.childid_arrive[i] = 4*id+i+1 < num_procs ? 4*id+i+1 : -1;
        treenode.childnotready[i] = 4*id+i+1 < num_procs ? 1 : -1;
    }

    for(i=0; i<2; i++)
        treenode.childid_wakeup[i] = 2*id+i+1 < num_procs ? 2*id+i+1 : -1; 
        
    wtime = MPI_Wtime();

    if(argc >=2) {
        if(strcmp(argv[argc-2], "-i") == 0) {
            num_iters = atoi(argv[argc-1]);
        } else {
            fprintf(stderr, "Usage: mpirun -np [num_procs] mpi_mcs -i [num_iters]");
            return 1;
        }
        if(num_iters < 0) {
            fprintf(stderr, "Usage: mpirun -np [num_procs] mpi_mcs -i [num_iters]");
            return 1;
        }
    }

    int j;
    for(j=0; j<num_iters; j++)   
        tree_barrier(&treenode); 


    wtime = MPI_Wtime() - wtime;
    printf("Processor %d took %fs to run with %d processors and %d iterations\n", id, wtime, num_procs, num_iters);

    MPI_Finalize();
    return 0;
}

int tournament(int argc, char *argv[]) {
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
   
    // Initialize the treenode for this processor.
    treenode_t treenode;
    treenode.processid = id;
    treenode.parentsense = 0;

    int i;
    for(i=0; i<4; i++) {
        treenode.childid_arrive[i] = 4*id+i+1 < num_procs ? 4*id+i+1 : -1;
        treenode.childnotready[i] = 4*id+i+1 < num_procs ? 1 : -1;
    }

    for(i=0; i<2; i++)
        treenode.childid_wakeup[i] = 2*id+i+1 < num_procs ? 2*id+i+1 : -1; 
        
    wtime = MPI_Wtime();

    if(argc >=2) {
        if(strcmp(argv[argc-2], "-i") == 0) {
            num_iters = atoi(argv[argc-1]);
        } else {
            fprintf(stderr, "Usage: mpirun -np [num_procs] mpi_mcs -i [num_iters]");
            return 1;
        }
        if(num_iters < 0) {
            fprintf(stderr, "Usage: mpirun -np [num_procs] mpi_mcs -i [num_iters]");
            return 1;
        }
    }

    int j;
    for(j=0; j<num_iters; j++)   
        tree_barrier(&treenode); 

    wtime = MPI_Wtime() - wtime;
    printf("Processor %d took %fs to run with %d processors and %d iterations\n", id, wtime, num_procs, num_iters);

    MPI_Finalize();
    return 0;
}

int main(int argc, char *argv[]) {
    printf("************************\n");
    printf("Running tournament barrier\n");
    printf("\n");
    tournament(argc, argv);
    printf("\n");

    printf("************************\n");
    printf("Running MCS barrier\n");
    mcs(argc, argv);

    return 0;
}
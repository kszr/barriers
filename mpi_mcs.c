/**
 * An implementation of an MCS barrier.
 *
 * @author: Abhishek Chatterjee [achatterjee32]
 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mpi.h"
#include "mpi_mcs.h"

/* Helper function prototypes */
int children_not_ready(treenode_t *treenode);

/* Variables */
static int sense = 1;
static int num_procs;

void tree_barrier(treenode_t *treenode) {
    MPI_Status mpi_status;
    int childbuffer[4]; // Buffer that each child in the arrival tree uses to signal this node.
    int buf; // Buffer for other signals.
    int i;

    // Spin until all children are ready.
    while(!children_not_ready(treenode)) {
        // Not shared memory, so the parent has to receive the request and proceed accordingly.
        int i;
        for(i=0; i<4; i++) {
            if(treenode->childid_arrive[i] != -1)
                MPI_Recv(&childbuffer[i], 1, MPI_INT, treenode->childid_arrive[i], 1, MPI_COMM_WORLD, &mpi_status);

            if(childbuffer[i] == I_AM_READY) {
                // printf("Process %d heard back from %d\n", treenode->processid, treenode->childid_arrive[i]);
                treenode->childnotready[i] = 0;
            }
        }
    }

    // printf("Processor %d: All my children are ready.\n", treenode->processid);

    // Signal to parent that I am ready.
    if(treenode->processid != 0) {
        childbuffer[(treenode->processid - 1) % 4] = I_AM_READY;
        int parentid_a = (treenode->processid-1)/4;
        MPI_Send(&childbuffer[(treenode->processid - 1) % 4], 1, MPI_INT, parentid_a, 1, MPI_COMM_WORLD);
    }

    // Reset childnotready array in preparation for the next barrier.
    for(i=0; i<4; i++)
        treenode->childnotready[i] = 4*treenode->processid+i+1 < num_procs ? 1 : -1;

    // If I am not the root, spin until my parent signals wakeup.
    if(treenode->processid != 0) {
        while(1) {
            buf = -1;
            int parentid_w = (treenode->processid-1)/2; // Different parent for wakeup;
            MPI_Recv(&buf, 1, MPI_INT, parentid_w, 1, MPI_COMM_WORLD, &mpi_status);
            if(buf == WAKEUP_SIGNAL)
                break;
        }
    }

    buf = WAKEUP_SIGNAL;
    if(treenode->childid_wakeup[0] != -1)
        MPI_Send(&buf, 1, MPI_INT, treenode->childid_wakeup[0], 1, MPI_COMM_WORLD);
    if(treenode->childid_wakeup[1] != -1)
        MPI_Send(&buf, 1, MPI_INT, treenode->childid_wakeup[1], 1, MPI_COMM_WORLD);

    sense = !sense; // May not be using sense at all.
}

/**
 * Returns true if any of treenode's children are not ready; false
 * if they are all ready.
 */
int children_not_ready(treenode_t *treenode) {
    int i;
    for(i=0; i<4; i++) {
        // Have to do it like this because we're using -1 to indicate that a child does not exist.
        if(treenode->childnotready[i] > 0)
            return 0;
    }
    return 1;
}

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

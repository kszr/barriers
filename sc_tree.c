/**
 * An implementation of a tournament barrier, following the algorithm 
 * in the Mellor-Crummey and Scott paper.
 * 
 * Implementation notes: The "tree" is maintained by a processor's notion of
 * what position in the tree it is in. A processor has a position element that 
 * represents its index in a hypothetical binary tree implemented as an array.
 * That is to say, there isn't an actual data structure representing the tree.
 * 
 * This implementation assumes that the number of processors is a power of two.
 *
 * @author: Abhishek Chatterjee [achatterjee32]
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "sc_tree.h"

process_t **process_array;
tree_node_t **tree;
static int sense;
static int num_procs;
static int tree_size;
static int round = 0;

/* Prototypes for helper functions */
int get_sender(int mypos, int round);
int get_recipient(int mypos, int round);
// static int initialize_tree();


int combining_barrier(process_t *process) {
    combining_barrier_aux(process);
    sense = !sense;
    return 0;
}

int combining_barrier_aux(process_t *process) {
    int buf;
    while(node->locksense != sense) {
        buf = (int) exp2(process->id);
        MPI_Send(&buf, 1, MPI_INT, process->id, MPI_COMM_WORL)
        MPI_Bcast(&buf, 1, MPI_INT, process->id, MPI_COMM_WORLD)
    }
    return 0;
}

int join_tournament(process_t *process) {
    join_tournament_aux(process);
    sense = !sense;
    round = 0;
    return 0;
}

int join_tournament_aux(process_t *process) {
    int buf=-1
    int locksense = !sense;
    while(locksense != sense) {
        if(process->id % 2 == 1 & process->round == round) {
            MPI_Send()
        }
    }
    return 0;  
}

// int combining_barrier_aux(node_t *node) {
//     // Spin on our locksense variable.
//     while(node->locksense != sense) {
//         // Needs to be atomic fetch_and_decrement.
//         if(count == 1) {
//             if(parent)
//                 combining_barrier_aux(parent);
//             count = k;  // Prepare for next barrier
//             node->locksense = !node->locksense; // Release waiting processors.
//         }
//     }
// }

// int initialize_tree() {
//     int i;
//     for(i=0; i<tree_size; i++) {
//         tree[i] = (tree_node_t *) malloc(sizeof(tree_node_t *));
//         tree[i]->k = 0;
//         tree[i]->process = NULL;

//         if(i==0) {
//             tree[i]->parent = NULL;
//         } else {
//             tree[i]->parent = tree[(i-1)/2];
//         }

//         if(2*i+1 < tree_size) {
//             tree[i]->left = tree[2*i+1];
//             tree[i]->k++;
//         } else {
//             tree[i]->left = NULL;
//         }

//         if(2*i+2 < tree_size) {
//             tree[i]->right = tree[2*i+2];
//             tree[i]->k++;
//         }
//     }
//     return 0;
// }

    
int main(int argc, char *argv[]) {
    int id;
    int ierr;
    double wtime;
    int tree_initialized = 0;

    printf("argc = %d\n", argc);

    // Initiaize MPI
    ierr = MPI_Init(&argc, &argv);

    // Get number of processes.
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Get individual process id.
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &id);

    tree_size = exp2(log2(num_procs)+ 1)-1; // Size of a complete binary tree is 2^(h+1)-1; size of level h is 2^h.

    // Initialize the process struct for this process.
    process_t process;
    process.id = id;
    process.round = 0;
    process.position = tree_size-num_procs+id;
    printf("Initialized process with id %d\n", d);

    join_tournament(&process);
    
    return 0;
}


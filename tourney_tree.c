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

#include "tourney_tree.h"

static int sense = 1;
static int num_procs;
static int tree_size;

/* Prototypes for helper functions */
static int get_loser(int mypos);
static int join_tournament_aux(processor_t *processor);

int join_tournament(processor_t *processor) {
    join_tournament_aux(processor);
    sense = !sense;
    return 0;
}

static int join_tournament_aux(processor_t *processor) {
    int buf;
    while(processor->locksense != sense) {
        buf = -1;
        if(processor->position % 2) {
            if(!processor->has_sent) {
                buf = processor->position;
                MPI_Bcast(&buf, 1, MPI_INT, processor->id, MPI_COMM_WORLD);
                processor->has_sent = 1;
            }
        } else {
            if(buf == get_loser(processor->position)) {
                processor->round += 1;
                processor->has_sent = 0;
                processor->position = (processor->position-1)/2;
            }
        }
        if(processor->round == 0)
            wakeup();
    }
    return 0;  
}

int wakeup() {
    printf("Wakeup\n");
    sense = !sense;
    return 0;
}

/**
 * Returns the statically assigned loser for a processor at position mypos
 * in the hypothetical tourney tree.
 */
static int get_loser(int mypos) {
    return mypos+1;
}
    
int main(int argc, char *argv[]) {
    int id;
    int ierr;
    double wtime;

    printf("argc = %d\n", argc);

    // Initiaize MPI
    ierr = MPI_Init(&argc, &argv);

    // Get number of processores.
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Get individual processor id.
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &id);

    tree_size = exp2(log2(num_procs)+ 1)-1; // Size of a complete binary tree is 2^(h+1)-1; size of level h is 2^h.
    printf("Tree size is %d\n", tree_size);

    // Initialize the processor struct for this processor.
    processor_t processor;
    processor.id = id;
    processor.round = 0;
    processor.position = tree_size-num_procs+id;
    processor.has_sent = 0;
    processor.locksense = 0;
    printf("Initialized processor with id %d\n", id);

    join_tournament(&processor);
    
    return 0;
}


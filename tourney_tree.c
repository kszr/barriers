/**
 * An implementation of a tournament barrier, following the algorithm 
 * in the Mellor-Crummey and Scott paper.
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
static int is_sender(processor_t *processor);
static int get_source(processor_t *processor);
static int get_dest(processor_t *processor);
static int join_tournament_aux(processor_t *processor);

int join_tournament(processor_t *processor) {
    join_tournament_aux(processor);
    sense = !sense;
    return 0;
}

static int join_tournament_aux(processor_t *processor) {
    int buf;
    MPI_Status mpi_result;
    while(processor->locksense != sense) {
        buf = -1;
        if(processor->round == (int) log2(num_procs)) {
            wakeup(processor, &buf);
            break;
        }
        if(is_sender(processor)) {
            if(!processor->has_sent) {
                buf = CONCEDE_SIGNAL;
                printf("Processor %d concedes to processor %d\n", processor->id, get_dest(processor));
                MPI_Send(&buf, 1, MPI_INT, get_dest(processor), 1, MPI_COMM_WORLD);
                processor->has_sent = 1;
            }
            MPI_Recv(&buf, 1, MPI_INT, get_dest(processor), 1, MPI_COMM_WORLD, &mpi_result);
            if(buf == WAKEUP_SIGNAL) {
                wakeup(processor, &buf);
                break;
            }
        } else {
            MPI_Recv(&buf, 1, MPI_INT, get_source(processor), 1, MPI_COMM_WORLD, &mpi_result);
            if(buf == CONCEDE_SIGNAL) {
                processor->round++;
                processor->has_sent = 0;
            }
        }
    }
    MPI_Finalize();
    return 0;  
}

int wakeup(processor_t *processor, int *buf) {
    while(processor->round != 0) {
        processor->round--;
        *buf = WAKEUP_SIGNAL;
        printf("Processor %d sends wakeup signal to processor %d\n", processor->id, get_source(processor));
        MPI_Send(buf, 1, MPI_INT, get_source(processor), 1, MPI_COMM_WORLD);
    }
    return 0;
}

/**
 * Returns 1 if the processor needs to send a concession signal in this round.
 */
static int is_sender(processor_t *processor) {
    int base = (int) exp2(processor->round+1);
    return processor->id % base == (int) exp2(processor->round) % base;
}

/**
 * Returns the id of the processor this processor is expecting to hear from
 * in this round.
 */
static int get_source(processor_t *processor) {
return processor->id + (int) exp2(processor->round);
}

/**
 * Returns the id of the processor this processor needs to communicate with
 * in this round.
 */
static int get_dest(processor_t *processor) {
    return processor->id - (int) exp2(processor->round);
}
    
int main(int argc, char *argv[]) {
    int id;
    int ierr;
    double wtime;

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
    processor.has_received = 0;
    processor.locksense = 0;
   
    join_tournament(&processor);
    
    return 0;
}


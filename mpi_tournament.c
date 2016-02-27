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
#include <string.h>

#include "mpi_tournament.h"

static int sense = 1;
static int num_procs;
static int tree_size;

/* Prototypes for helper functions */
static int is_sender(processor_t *processor);
static int is_receiver(processor_t *processor);
static int get_source(processor_t *processor);
static int get_dest(processor_t *processor);
static int join_tournament_aux(processor_t *processor);

/**
 * Enters the processor into the tournament. Flips the sense flag at the end
 * in preparation for the next barrier.
 */
int join_tournament(processor_t *processor) {
    join_tournament_aux(processor);
    sense = !sense;
    return 0;
}

/**
 * A helper function that represents a single instance of the tournament barrier.
 */
static int join_tournament_aux(processor_t *processor) {
    int buf;
    MPI_Status mpi_result;
    while(processor->locksense != sense) {
        buf = -1;
        // The processor that reaches the root initiates wakeup.
        if(processor->round == (int) log2(num_procs)) {
            wakeup(processor, &buf);
            processor->round = -1; // Ensures that the processor does not enter the send and or receive blocks on the way out.
            processor->locksense = !processor->locksense;
        }
        // Statically determined loser in a round needs to concede to a statically determined winner.
        if(is_sender(processor)) {
            if(!processor->has_sent) {
                buf = CONCEDE_SIGNAL;
                // printf("Processor %d concedes to processor %d\n", processor->id, get_dest(processor));
                MPI_Send(&buf, 1, MPI_INT, get_dest(processor), 1, MPI_COMM_WORLD);
                processor->has_sent = 1;
            }
            // Receives a wakeup signal from the processor that beat it.
            MPI_Recv(&buf, 1, MPI_INT, get_dest(processor), 1, MPI_COMM_WORLD, &mpi_result);
            if(buf == WAKEUP_SIGNAL) {
                wakeup(processor, &buf);
                processor->locksense = !processor->locksense;
                processor->has_sent = 0;           
            }
        } else if(is_receiver(processor)) {
            // Receives a concession signal from the statically determined loser in a round, and advances to the next round.
            MPI_Recv(&buf, 1, MPI_INT, get_source(processor), 1, MPI_COMM_WORLD, &mpi_result);
            if(buf == CONCEDE_SIGNAL) {
                processor->round++;
                processor->has_sent = 0;
            }
        }
    }
    processor->round = 0;
    return 0;  
}

/**
 * processor iteratively sends a wakeup signal to every processor that conceded to it during the tournament.
 */
int wakeup(processor_t *processor, int *buf) {
    while(processor->round != 0) {
        processor->round--;
        *buf = WAKEUP_SIGNAL;
        // printf("Processor %d sends wakeup signal to processor %d\n", processor->id, get_source(processor));
        MPI_Send(buf, 1, MPI_INT, get_source(processor), 1, MPI_COMM_WORLD);
    }
    return 0;
}

/**
 * Returns 1 if the processor needs to send a concession signal in this round. Number obtained from
 * Hensgen, Finkel, and Manber's barrier as given in the MCS paper.
 */
static int is_sender(processor_t *processor) {
    int base = (int) exp2(processor->round+1);
    return processor->round < 0 ? 0 : processor->id % base == (int) exp2(processor->round) % base;
}

/**
 * Returns 1 if the processor expects to receive a concession signal in this round.
 */
static int is_receiver(processor_t *processor) {
    return processor->round < 0 ? 0 : !is_sender(processor);
}

/**
 * Returns the id of the processor this processor is expecting to hear from
 * in this round. Number from H, F, and M's barrier.
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

    int i;
    for(i=0; i<num_iters; i++)   
        join_tournament(&processor); 


    wtime = MPI_Wtime() - wtime;
    printf("Processor %d took %fs to run with %d processors and %d iterations\n", id, wtime, num_procs, num_iters);

    MPI_Finalize();
    return 0;
}


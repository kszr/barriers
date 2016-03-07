/**
 * An implementation of an MPI-OpenMP barrier that combines the MPI tournament barrier and 
 * the OpenMP dissemination barrier we wrote separately.
 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include "mpi_tournament.h"

#define MAX_PROCS 1000

typedef struct flags {
    int myflags[2][MAX_PROCS];
    int *partnerflags[2][MAX_PROCS];
} flags;

static int sense = 1;
static int num_procs;
static int num_threads;

/* Prototypes for helper functions */
static int is_sender(processor_t *processor);
static int is_receiver(processor_t *processor);
static int get_source(processor_t *processor);
static int get_dest(processor_t *processor);
static int join_tournament_aux(processor_t *processor);
static void initialize_omp_barrier(flags *allnodes, int num_threads, int num_rounds);
static void omp_dissemination_barrier (flags *localflags, int *parity, int *sense, int num_rounds);
static int run_omp_barrier(int num_threads);


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
    run_omp_barrier(num_threads);
    int buf;
    MPI_Status mpi_result;
    while(processor->locksense != sense) {
        buf = -1;
        // The processor that reaches the root initiates wakeup.
        if(processor->round == (int) ceil(log2(num_procs))) {
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
        } else {
            processor->round++;
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
        if(get_source(processor) < num_procs)
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
    return processor->round < 0 ? 0 : (!is_sender(processor)) && (get_source(processor) < num_procs);
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

static void initialize_omp_barrier(flags *allnodes, int num_threads, int num_rounds) {
    int i, k;
    for(i=0; i<num_threads; i++)
        for(k=0; k<num_rounds; k++) {
            allnodes[i].myflags[0][k] = 0;
            allnodes[i].myflags[1][k] = 0;
    }
}

static void omp_dissemination_barrier (flags *localflags, int *parity, int *dissemination_sense, int num_rounds) {
    int k;
    for (k=0; k<num_rounds; k++) {
        #pragma omp critical
            *localflags->partnerflags[*parity][k] = *dissemination_sense;
        while(localflags->myflags[*parity][k] != *dissemination_sense) {}
    }
    
    if(*parity == 1)
        *dissemination_sense = !*dissemination_sense;
    *parity = 1 - *parity;

}

/**
 * A function that prepares and calls the omp dissemination barrier we wrote separately.
 */
static int run_omp_barrier(int num_threads) {
    /*Serial Code started*/
   
    flags allnodes[num_threads];
    int num_rounds = ceil (log(num_threads)/log(2));
    omp_set_num_threads(num_threads);
    double T1, T2;
    	
     T1 = omp_get_wtime();
                
    /*Parallel Code started*/
    #pragma omp parallel shared (allnodes, num_rounds)
    {
        int thread_id = omp_get_thread_num();
        long i;
        int j, r, k;
        flags *localflags = &allnodes[thread_id];
        int parity = 0;
        int dissemination_sense = 1;
        #pragma omp single
            initialize_omp_barrier(allnodes, num_threads, num_rounds);
        
            #pragma omp critical
                for (j=0; j<num_threads; j++)
                    for (k=0; k<num_rounds; k++) {
                        if (j == ((thread_id + (int)(pow(2,k))) % num_threads)) {
                            for(r=0; r<2; r++)
                                allnodes[thread_id].partnerflags[r][k] = &allnodes[j].myflags[r][k];
                        }
                    }           
           omp_dissemination_barrier(localflags, &parity, &dissemination_sense, num_rounds);
       }

     T2 = omp_get_wtime();
//     printf("took %fs to run with %d processors\n", (T2-T1), num_threads);
    
    return 0;
}
    
int main(int argc, char *argv[]) {
    int id;
    int ierr;
    double wtime;
    int num_iters = 1000000;

    // Initiaize MPI
    ierr = MPI_Init(&argc, &argv);

    // Get number of processores.
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Get individual processor id.
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &id);
    
    // Initialize the processor struct for this processor.
    processor_t processor;
    processor.id = id;
    processor.round = 0;
    processor.has_sent = 0;
    processor.locksense = 0;

    if(argc >=2) {
        if(strcmp(argv[argc-2], "-i") == 0) {
            num_threads = atoi(argv[argc-1]);
        } else {
            fprintf(stderr, "Usage: mpirun -np [num_procs] mpi_omp_barrier -i [num_threads]");
            return 1;
        }
        if(num_threads < 0) {
            fprintf(stderr, "Usage: mpirun -np [num_procs] mpi_omp_barrier -i [num_threads]");
            return 1;
        }
    }

    wtime = MPI_Wtime();

    int i;
    for(i=0; i<num_iters; i++)   
        join_tournament(&processor); 

    wtime = MPI_Wtime() - wtime;

    printf("Processor %d took %fs to run with %d processors and %d threads and %d iterations\n", id, wtime, num_procs, num_threads, num_iters);

    MPI_Finalize();
    return 0;
}

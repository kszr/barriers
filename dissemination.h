#ifndef DISSEMINTAION_H
#define DISSEMINATION_H

#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

// This signals tell the recipient of a message what to do.
static const int CONCEDE_SIGNAL = 1;
static const int WAKEUP_SIGNAL = 2;

typedef struct processor_s {
    int id; // Processor id
    int round;  // The round this processor is currently in.
    int has_sent; // 1 if the processor has sent a message already; 0 otherwise.
    int has_received;
    int locksense; // The variable this processor needs to spin on.
} processor_t;

int join_tournament(processor_t *processor);
int join_tree(processor_t *processor);
int wakeup(processor_t *processor, int *buf);

#endif

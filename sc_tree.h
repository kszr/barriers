#ifndef SC_TREE_H
#define SC_TREE_H

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

//#include "stack.h"
typedef struct process_s {
    int id; // Processor id
    int locksense;  // Initially false.
} process_t;

typedef struct tree_node_s {
    int id;
    int k;  // Fan-in of this node
    int count; // Count - initially k
    struct tree_node_s *parent;
    struct tree_node_s *left;
    struct tree_node_s *right;
    process_t *process; // The process occupying this tree node.
} tree_node_t;

int combining_barrier();
int combining_barrier_aux(tree_node_t *node);
int join_tree(process_t *process);

#endif

#ifndef MPI_MCS_H
#define MPI_MCS_H

typedef struct treenode_s {
    int parentsense;
    int parentpointer; // Not an actual pointer in a message passing-based algorithm
    int *childpointers; // Stores the id of its children.
    int *havechild; // Stores the id of its four children; -1 if there is no child at this position.
    int *childnotready;
    int dummy;
} treenode_t;

void tree_barrier();

#endif
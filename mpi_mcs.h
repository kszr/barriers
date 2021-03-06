/**
 * @author: Abhishek Chatterjee [achatterjee32]
 */
#ifndef MPI_MCS_H
#define MPI_MCS_H

// Signals
static const int I_AM_READY_MCS = 0;
static const int WAKEUP_SIGNAL_MCS = 1;

typedef struct treenode_s {
    int processid;
    int parentsense;
    int childid_wakeup[2]; // Stores the id of its two children in the wakeup tree.
    int childid_arrive[4]; // Stores the id of its four children in the arrival tree; -1 if there is no child at this position.
    int childnotready[4]; // Stores child id (if any) and: -1 if no corresponding child; 0 if child not ready; 1 if child is ready. 
} treenode_t;

void tree_barrier(treenode_t *treenode);

#endif

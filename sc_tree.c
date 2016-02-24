/**
 * An implementation of a software combining tree barrier with wakeup,
 * following the algorithm in the Mellor-Crummey and Scott paper.
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


/* Prototypes for helper functions */
static int initialize_tree();

/*
int combining_barrier() {
    combining_barrier_aux(mynode);
    sense = !sense;
}

int combining_barrier_aux(node_t *node) {
    // Spin on our locksense variable.
    while(node->locksense != sense) {
        // Needs to be atomic fetch_and_decrement.
        if(count == 1) {
            if(parent)
                combining_barrier_aux(parent);
            count = k;  // Prepare for next barrier
            node->locksense = !node->locksense; // Release waiting processors.
        }
    }
}

static int join_tree(node_t *node, node_t *curr_root) {
    if(!curr_root->left || !curr_root->right) {
        if(!curr_root->left) {
            curr_root->left = node;
            node->parent = curr_root;
        } else {
            curr_root->right = node;
            node->parent = curr_root;
        }
        curr_root->k++;
        return 1;
    }

    return join_tree(node, curr_root->left) || join_tree(node, curr_root->right);
} */


int initialize_tree() {
    int i;
    for(i=0; i<tree_size; i++) {
        tree[i] = (tree_node_t *) malloc(sizeof(tree_node_t *));
        tree[i]->k = 0;
        tree[i]->process = NULL;

        if(i==0) {
            tree[i]->parent = NULL;
        } else {
            tree[i]->parent = tree[(i-1)/2];
        }

        if(2*i+1 < tree_size) {
            tree[i]->left = tree[2*i+1];
            tree[i]->k++;
        } else {
            tree[i]->left = NULL;
        }

        if(2*i+2 < tree_size) {
            tree[i]->right = tree[2*i+2];
            tree[i]->k++;
        }
    }
    return 0;
}

int join_tree(process_t *process) {
    int id = process->id;
    tree[tree_size-num_procs+id]->process = process;
    printf("Process %d joined tree.\n", id);
    return 0;
}
    
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

    // Initialize the process struct for this process.
    process_t *process = (process_t *) malloc(sizeof(process_t));
    process->id = id;
    process->locksense = 0;

    
    if(id == 0) {
        process_array = (process_t **) malloc(sizeof(process_t *)*num_procs);
        tree_size = exp2(log2(num_procs)+ 1)-1; // Size of a complete binary tree is 2^(h+1)-1; size of level h is 2^h.
        tree = (tree_node_t **) malloc(sizeof(tree_node_t **)*tree_size);
        initialize_tree();
        tree_initialized = 1;
        printf("Process %d initialized tree.\n", id);
    }

    process_array[id] = process;
    printf("Process %d checking in.\n", id);
    join_tree(process);
    
    return 0;
}


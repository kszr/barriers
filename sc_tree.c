/**
 * An implementation of a software combining tree barrier with wakeup,
 * following the algorithm in the Mellor-Crummey and Scott paper.
 * @author: Abhishek Chatterjee [achatterjee32]
 */

#include "sc_tree.h"

process_t **process_array;
static int sense;
static tree_node_t *root;

/* Prototypes for helper functions */
static int initialize_tree();
static int initialize_tree_recursive();

int combining_barrier() {
    combining_barrier_aux(mynode);
    sense = !sense;
}
/*
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
    root = malloc(sizeof(tree_node_s));
    root->parent = NULL;
    return initialize_tree_recursive(root);
}

int initialize_tree_recursive(tree_node_s *curr) {

}

int main(int argc, char *argv[]) {
    int id;
    int ierr;
    int num_procs;
    double wtime;

    // Initiaize MPI
    ierr = MPI_Init(&argc, &argv);

    // Get number of processes.
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Get individual process id.
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // Initialize the process struct for this process.
    proces_t *process = (process_t *) malloc(sizeof(process_t));
    process->id = id;
    process->locksense = 0;

    if(id == 0) {
        process_array = (process_t **) malloc(sizeof(process_t *)*num_procs);
        initialize_tree();
    }

    process_array[id] = process;

    /*
    if(id == 0)
        nodes_array = (node_t **) malloc(sizeof(node_t *)*num_procs);
    */


    nodes_array[id] = node;

    if(!root) {
        root = node;
    } else {
        join_tree(node, root);
    }

    return;
}


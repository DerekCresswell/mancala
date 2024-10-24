
#include "gametree.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

void Node_cleanup(Node *node, void (*free_state) (void *state)) {

    for (int i = 0; i < node->number_successors; i++) {
        Node_cleanup(node->successors + i, free_state);
    }

    if (node->game_state) {
        free_state(node->game_state);
    }

    free(node->successors);

}

void MinMaxSearch_print_stats(MinMaxSearch *search) {
    printf(
        "%d Nodes generated and %d explored in %dms and %dus.\n",
        search->nodes_generated,
        search->nodes_explored,
        search->elapsed_time_ms,
        search->elapsed_time_us
    );
}

int _max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int _min(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

/**
 * The inner search function which returns utility values instead of nodes.
 */
int _MinMaxSearch_search_inner(MinMaxSearch *search, Node *root, int max_player, int depth) {

    // We are exploring a new node.
    search->nodes_explored++;

    // Check if our node is at depth or terminal.
    int at_depth = depth <= 0;
    int is_terminal = search->is_terminal(root->game_state);
    if (at_depth || is_terminal) {
        return search->utility(root->game_state);
    }

    // Generate the successors of this node.
    int number_successors = MinMaxSearch_generate_successor_nodes(search, root);

    // Now, we may explore the successor nodes.
    int is_max = max_player == search->get_turn(root->game_state);
    int (*eval_function)(int, int);

    if (is_max) {
        eval_function = &_max;
    } else {
        eval_function = &_min;
    }

    int best_utility = 0;
    for (int i = 0; i < number_successors; i++) {

        int utility = _MinMaxSearch_search_inner(search, root->successors + i, max_player, depth - 1);
        best_utility = eval_function(best_utility, utility);

    }

    return best_utility;

}

Node *MinMaxSearch_search(MinMaxSearch *search, Node *root) {

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

    // We must generate the successors of the root node and run our search on it.
    // This assumes we are not at a terminal node.
    int number_successors = MinMaxSearch_generate_successor_nodes(search, root);

    int max_player = search->get_turn(root->game_state);

    // Return the node that had the highest utility.
    int index_of_highest_utility = 0;
    int highest_utility = -1;
    for (int i = 0; i < number_successors; i++) {

        int utility = _MinMaxSearch_search_inner(search, root->successors + i, max_player, search->depth - 1);

        if (utility > highest_utility) {
            highest_utility = utility;
            index_of_highest_utility = i;
        }

    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);

    long difference_s = end_time.tv_sec - start_time.tv_sec;
    long difference_ns = end_time.tv_nsec - start_time.tv_nsec;

    search->elapsed_time_ms = difference_s * 1000 + (difference_ns / 1000000);
    search->elapsed_time_us = difference_s * 1000000 + (difference_ns / 1000);
    search->elapsed_time_us -= search->elapsed_time_ms * 1000;

    return root->successors + index_of_highest_utility;

}

int MinMaxSearch_generate_successor_nodes(MinMaxSearch *search, Node *root) {

    void **successors;
    int number_successors = search->get_successors(root->game_state, &successors);

    // The node is not terminal, so there must be at least one successor.
    // We will turn these successors into proper nodes.
    root->number_successors = number_successors;
    root->successors = malloc(sizeof(Node) * number_successors);
    for (int i = 0; i < number_successors; i++) {

        root->successors[i].game_state = successors[i];
        root->successors[i].number_successors = -1;
        root->successors[i].successors = NULL;

    }

    // We have generated more nodes.
    search->nodes_generated += number_successors;

    // Free the successors list as we now have the pointers to the game states saved.
    free(successors);

    return number_successors;

}

void MinMaxSearch_reset_stats(MinMaxSearch *search) {

    search->nodes_generated = 0;
    search->nodes_explored = 0;
    search->elapsed_time_ms = 0;
    search->elapsed_time_us = 0;

}
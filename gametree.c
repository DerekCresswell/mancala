
#include "gametree.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <limits.h>

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
        search->stats.nodes_generated,
        search->stats.nodes_explored,
        search->stats.elapsed_time_ms,
        search->stats.elapsed_time_us
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

int _time_left(struct timespec start_time, int allowed_ms) {

    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
    long ms_elapsed = 1000 * (end_time.tv_sec - start_time.tv_sec);
    long ns_elapsed = end_time.tv_nsec - start_time.tv_nsec;

    if (ns_elapsed < 0) {
        ms_elapsed -= 1000;
    }

    return allowed_ms - ms_elapsed;

}

/**
 * The inner search function which returns utility values instead of nodes.
 */
int _MinMaxSearch_search_inner(MinMaxSearch *search, Node *root, int max_player, int depth, struct timespec start_time) {

    // We are exploring a new node.
    search->stats.nodes_explored++;

    // Check if our node is at depth, terminal, or we are out of time.
    int at_depth = depth <= 0;
    int is_terminal = search->is_terminal(root->game_state);
    int is_time_left = _time_left(start_time, search->options.time_limit_in_ms) > 0;
    if (at_depth || is_terminal || !is_time_left) {
        return search->utility(root->game_state, max_player);
    }

    int is_max = max_player == search->get_turn(root->game_state);

    // Check if this is a dead state.
    if (search->options.dead_state_pruning) {

        int is_dead = search->is_dead_state(root->game_state, search->get_turn(root->game_state));
        if (is_dead && is_max) {
            return INT_MIN;
        }
        if (is_dead) {
            return INT_MAX;
        }

    }

    // Generate the successors of this node.
    int number_successors = MinMaxSearch_generate_successor_nodes(search, root);

    // Now, we may explore the successor nodes.
    int (*eval_function)(int, int);

    int best_utility;
    if (is_max) {
        eval_function = &_max;
        best_utility = INT_MIN;
    } else {
        eval_function = &_min;
        best_utility = INT_MAX;
    }

    for (int i = 0; i < number_successors; i++) {

        int next_depth = depth - 1;

        int utility = _MinMaxSearch_search_inner(search, root->successors + i, max_player, next_depth, start_time);
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

    int max_depth = search->options.max_depth;

    // Perform iterative deepening if required.
    // These are sane defaults that will perform a single max depth search.
    int starting_depth = max_depth;
    int depth_step = 1;
    if (search->options.iterative_deepening) {
        starting_depth = search->options.starting_depth;
        depth_step = search->options.depth_step;
    }

    int index_of_highest_utility = 0;
    int highest_utility = INT_MIN;
    for (int current_search_depth = starting_depth; current_search_depth <= max_depth; current_search_depth += depth_step) {

        search->depth = current_search_depth;

        // Return the node that had the highest utility.
        int is_time_left = 1;
        for (int i = 0; i < number_successors; i++) {

            int next_depth = search->depth - 1;

            int utility = _MinMaxSearch_search_inner(search, root->successors + i, max_player, next_depth, start_time);

            if (utility > highest_utility) {
                highest_utility = utility;
                index_of_highest_utility = i;
            }

        }

        if (!is_time_left) {
            break;
        }

    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);

    // Calculate the time taken.
    long difference_s = end_time.tv_sec - start_time.tv_sec;
    long difference_ns = end_time.tv_nsec - start_time.tv_nsec;

    if (difference_ns < 0) {
        difference_s -= 1;
        difference_ns += 1000000000;
    }

    search->stats.elapsed_time_ms = difference_s * 1000 + (difference_ns / 1000000);
    search->stats.elapsed_time_us = difference_s * 1000000 + (difference_ns / 1000);
    search->stats.elapsed_time_us -= search->stats.elapsed_time_ms * 1000;

    // Return the best successor node.
    return root->successors + index_of_highest_utility;

}

int MinMaxSearch_generate_successor_nodes(MinMaxSearch *search, Node *root) {

    // Only generate nodes if we need to.
    if (root->number_successors >= 0) {
        return root->number_successors;
    }

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
    search->stats.nodes_generated += number_successors;

    // Free the successors list as we now have the pointers to the game states saved.
    free(successors);

    return number_successors;

}

void MinMaxSearch_reset_stats(MinMaxSearch *search) {

    search->stats.nodes_generated = 0;
    search->stats.nodes_explored = 0;
    search->stats.elapsed_time_ms = 0;
    search->stats.elapsed_time_us = 0;

}
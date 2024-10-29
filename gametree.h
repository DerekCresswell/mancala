
typedef struct _Node {

    void *game_state;

    int number_successors;
    struct _Node *successors;

} Node;

/**
 * Cleans up successor nodes.
 */
void Node_cleanup(Node *node, void (*free_state) (void *state));

typedef struct {

    // Search options.
    // ---------------
    struct {

        // Absolute maximum depth to search.
        int max_depth;

        // Enables and modifies iterative deepening.
        int iterative_deepening;
        int starting_depth;
        int depth_step;
        int time_limit_in_ms; // If negative, there is no limit.

        // Enables pruning techniques.
        int dead_state_pruning;
        int alpha_beta_pruning;

    } options;

    int depth;

    // Game functions.
    int (*utility) (void *state, int for_player);
    int (*is_terminal) (void *state);
    int (*get_turn) (void *state);
    int (*get_successors) (void *state, void ***successors);
    void (*free_state) (void *state);
    int (*is_dead_state) (void *state, int for_player);

    // Stats.
    struct {
        int nodes_generated;
        int nodes_explored;
        int elapsed_time_ms;
        int elapsed_time_us;
    } stats;

} MinMaxSearch;

/**
 * Prints the statistics from the last search.
 *
 * Be sure to clear the stats using _reset_stats before running a new search.
 */
void MinMaxSearch_print_stats(MinMaxSearch *search);

/**
 * Returns the successor node of root that will yield the maximal return.
 * Only explores up to search.depth nodes deep.
 */
Node *MinMaxSearch_search(MinMaxSearch *search, Node *root);

/**
 * Generates the successors for the root node.
 * Returns the number of successors.
 */
int MinMaxSearch_generate_successor_nodes(MinMaxSearch *search, Node *root);

/**
 * Resets the stats of the search object.
 */
void MinMaxSearch_reset_stats(MinMaxSearch *search);

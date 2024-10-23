
typedef struct _Node {

    void *game_state;

    int number_successors;
    struct _Node *successors;

} Node;

typedef struct {

    int (*utility) (void *state);
    int (*is_terminal) (void *state);
    int (*get_turn) (void *state);
    int (*get_successors) (void *state, void ***successors);

    // Stats.
    int nodes_generated;
    int nodes_explored;

} MinMaxSearch;

/**
 * Returns the successor node of root that will yield the maximal return.
 * Only explores up to depth nodes deep.
 */
Node *MinMaxSearch_search(MinMaxSearch *search, Node *root, int depth);

/**
 * Generates the successors for the root node.
 * Returns the number of successors.
 */
int MinMaxSearch_generate_successor_nodes(MinMaxSearch *search, Node *root);

/**
 * Resets the stats of the search object.
 */
void MinMaxSearch_reset_stats(MinMaxSearch *search);
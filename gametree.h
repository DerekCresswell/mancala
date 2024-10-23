
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

    int depth;

    int (*utility) (void *state);
    int (*is_terminal) (void *state);
    int (*get_turn) (void *state);
    int (*get_successors) (void *state, void ***successors);
    void (*free_state) (void *state);

    // Stats.
    int nodes_generated;
    int nodes_explored;

} MinMaxSearch;

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

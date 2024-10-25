
typedef struct {

    int length;
    int *lanes[2];
    int stores[2];

    int turn; // 0 or 1 for the next player to play.

} GameBoard;

/**
 * Allocates and deallocates resources for a game of Mancala.
 */
GameBoard *GameBoard_create(int length, int starting_seeds);
GameBoard *GameBoard_copy(GameBoard *board);
void GameBoard_delete(GameBoard *board);

/**
 * Prints the current state of the gameboard like so:
 *
 * 0 + 3 3 3 3 3 3 +
 *   0             0
 *   + 3 3 3 3 3 3 + 1  0 to play.
 *
 * The top lane is player 0's lane from left to right.
 * The store on the right side of the board is then player 0's.
 * Player 1's lane is on the bottom from right to left.
 * The store on the left side of the board is then player 1's.
 */
void GameBoard_print(GameBoard *board);
/**
 * Plays out the turn of the next by emptying the specified pit.
 * The seeds from that pit are placed one-by-one into the following pits and the
 * players store but not the opponents store.
 *
 * The following may occur:
 *  - If the last seed is placed into the players store, it will be their turn again.
 *  - If the last seed falls an empty pit of the players and the adjacent opponents
 *    pit is non empty, the seeds in both of these pits are captured for the player.
 *
 * Returns the next player to play.
 *
 * Assumes the input is a valid play (see `_is_valid_play`).
 */
int GameBoard_play_turn(GameBoard *board, int pit_to_play);

/**
 * Checks if the suggested play is a valid turn.
 */
int GameBoard_is_valid_play(GameBoard *board, int pit_to_play);

/**
 * Returns 1 if the game is over or 0 if not.
 *
 * A game is over when either player has no seeds in their lane.
 * The winner is whichever player has more seeds in their store at this time.
 */
int GameBoard_is_game_over(GameBoard *board);

/**
 * Returns which player has more seeds in their store or -1 in a tie.
 */
int GameBoard_winner_is(GameBoard *board);

/**
 * Returns the score for the given player as if the game is over.
 */
int GameBoard_score_of(GameBoard *board, int player);

/**
 * Returns the number of successor states to this one.
 * The provided pointer will point to the list of these states.
 *
 * The list will point to a list of pointers to gameboards.
 * The list will be allocated within this function as well as each new gameboard.
 * It is responsibility of the caller to free all of this.
 */
int GameBoard_get_successors(GameBoard *board, GameBoard ***successors);

/**
 * Returns the utility for the current player.
 */
int GameBoard_utility(GameBoard *board, int for_player);

/**
 * Returns the current player;
 */
int GameBoard_current_turn(GameBoard *board);
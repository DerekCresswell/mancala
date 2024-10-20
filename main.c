
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {

    int length;
    int *lanes[2];
    int stores[2];

    int turn; // 0 or 1 for the next player to play.

} GameBoard;

int GameBoard_is_game_over(GameBoard *board);
int GameBoard_winner_is(GameBoard *board);

GameBoard *GameBoard_create(int length, int starting_seeds) {

    GameBoard *board = malloc(sizeof(GameBoard));

    if (board == NULL) {
        printf("Failed to allocate game board.\n");
        return NULL;
    }

    // Set defaults of board.
    board->length = length;

    board->lanes[0] = malloc(sizeof(int) * length);
    board->lanes[1] = malloc(sizeof(int) * length);
    // TODO: Safe.

    for (int i = 0; i < length; i++) {
        board->lanes[0][i] = starting_seeds;
        board->lanes[1][i] = starting_seeds;
    }

    board->stores[0] = 0;
    board->stores[1] = 0;

    board-> turn = 0;

    return board;

}

void GameBoard_delete(GameBoard *board) {

    free(board->lanes[0]);
    free(board->lanes[1]);
    free(board);

}

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
void GameBoard_print(GameBoard *board) {

    // Print first lane.
    printf("0 + %d", board->lanes[0][0]);
    for (int i = 1; i < board->length; i++) {
        printf(" %d", board->lanes[0][i]);
    }
    printf(" +\n");

    // Print stores.
    printf("  %d%*c%d\n", board->stores[1], board->length * 2 + 1, ' ', board->stores[0]);

    // Print last lane.
    printf("  + %d", board->lanes[1][board->length - 1]);
    for (int i = board->length - 2; i >= 0; i--) {
        printf(" %d", board->lanes[1][i]);
    }
    printf(" + 1");

    if (GameBoard_is_game_over(board)) {
        printf("\t%d won.\n", GameBoard_winner_is(board));
    } else {
        printf("\t%d to play.\n", board->turn);
    }

}

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
int GameBoard_play_turn(GameBoard *board, int pit_to_play) {

    // Constraints:
    //  * pit_to_play < board->length
    //  * board->lane_(board->turn) > 0

    int starting_turn = board->turn;

    int *playing_lane = board->lanes[starting_turn];
    int *playing_store = &(board->stores[starting_turn]);

    // Empty the played pit.
    int seeds_left = playing_lane[pit_to_play];
    playing_lane[pit_to_play] = 0;

    // Advance the pit index past the emptied one.
    pit_to_play++;

    // Distribute the seeds into the next pits.
    int current_turn = starting_turn;
    while (seeds_left > 0) {

        if (pit_to_play < board->length) {

            playing_lane[pit_to_play]++;
            pit_to_play++;
            seeds_left--;

        } else {

            // Place a pit in this players store.
            if (current_turn == starting_turn) {
                (*playing_store)++;
                seeds_left--;
            }

            // Swap to the other lane.
            pit_to_play = 0;
            current_turn = (current_turn + 1) % 2;
            playing_lane = board->lanes[current_turn];

        }

    }

    // Check for end-of-turn conditions.
    if (current_turn != starting_turn && pit_to_play == 0) {
        // Ended in own store. The player takes another turn.
    } else {
        board->turn = (board->turn + 1) % 2;
    }

    if (current_turn == starting_turn && pit_to_play > 0) {

        int landed_in_pit_index = pit_to_play - 1;
        int landed_in_pit = playing_lane[landed_in_pit_index];
        int opponents_turn = (starting_turn + 1) % 2;
        int adjacent_pit_index = board->length - landed_in_pit_index - 1;
        int adjacent_pit = board->lanes[opponents_turn][adjacent_pit_index];

        if (adjacent_pit > 0 && landed_in_pit == 1) {

            board->lanes[opponents_turn][adjacent_pit_index] = 0;
            playing_lane[landed_in_pit_index] = 0;
            (*playing_store) += adjacent_pit + 1;

        }

    }

    return board->turn;

}

/**
 * Checks if the suggested play is a valid turn.
 */
int GameBoard_is_valid_play(GameBoard *board, int pit_to_play) {

    // Check that the pit is a valid index.
    int valid_index = pit_to_play >= 0 && pit_to_play < board->length;

    // Check that the pit has a non-zero number of seeds.
    int valid_pit = board->lanes[board->turn][pit_to_play] > 0;

    return valid_index && valid_pit;

}

/**
 * Returns 1 if the game is over or 0 if not.
 *
 * A game is over when either player has no seeds in their lane.
 * The winner is whichever player has more seeds in their store at this time.
 */
int GameBoard_is_game_over(GameBoard *board) {

    int *lane_0 = board->lanes[0];
    int *lane_1 = board->lanes[1];

    int lane_0_empty = 1;
    int lane_1_empty = 1;

    for (int i = 0; i < board->length; i++) {

        lane_0_empty &= (lane_0[i] == 0);
        lane_1_empty &= (lane_1[i] == 0);

    }

    return lane_0_empty || lane_1_empty;

}

/**
 * Returns which player has more seeds in their store or -1 in a tie.
 */
int GameBoard_winner_is(GameBoard *board) {

    if (board->stores[0] > board->stores[1]) {
        // Player 0 won.
        return 0;
    }

    if (board->stores[0] == board->stores[1]) {
        // Tie.
        return -1;
    }

    // Player 1 won.
    return 1;

}

/* --- */

void run_console_game(int board_length, int starting_seeds) {

    // Initialize a game of mancala.
    GameBoard *board = GameBoard_create(board_length, starting_seeds);
    if (board == NULL) {
        return;
    }

    // Run main game loop.
    GameBoard_print(board);
    printf("\n");

    while (!GameBoard_is_game_over(board)) {

        int current_turn = board->turn;
        printf("Player %d's turn: ", current_turn);

        int pit_to_play = -1;
        scanf("%d", &pit_to_play);
        while (!GameBoard_is_valid_play(board, pit_to_play)) {
            printf("Invalid play. Try again: ");
            scanf("%d", &pit_to_play);
        }

        GameBoard_play_turn(board, pit_to_play);

        GameBoard_print(board);
        printf("\n");

    }

    int winner = GameBoard_winner_is(board);
    if (winner == -1) {
        printf("\nThe game is a draw!\n");
    } else {
        printf("\nPlayer %d has won!\n", winner);
    }

    // Clean up and exit.
    GameBoard_delete(board);

}

int main(int argc, char** argv) {

    int board_length = 6;
    int starting_seeds = 3;

    run_console_game(board_length, starting_seeds);

    return 0;

}
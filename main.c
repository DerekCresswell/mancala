
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {

    int length;
    int *lanes[2];
    int stores[2];

    int turn; // 0 or 1 for the next player to play.

} GameBoard;

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
    printf("  %d           ", board->stores[1]);
    printf("  %d\n", board->stores[0]);

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

        int landed_in_pit = pit_to_play - 1;
        int opponents_turn = (starting_turn + 1) % 2;
        int adjacent_pit_index = board->length - landed_in_pit - 1;
        int adjacent_pit = board->lanes[opponents_turn][adjacent_pit_index];

        if (adjacent_pit > 0) {

            board->lanes[opponents_turn][adjacent_pit_index] = 0;
            playing_lane[landed_in_pit] = 0;
            (*playing_store) += adjacent_pit + 1;

        }

    }

    return board->turn;

}

/**
 * Returns 1 if the game is over or 0 if not.
 *
 * A game is over when the player to play has no seeds in their lane.
 * The winner is whichever player has more seeds in their store at this time.
 */
int GameBoard_is_game_over(GameBoard *board) {

    int *lane = board->lanes[board->turn];

    for (int i = 0; i < board->length; i++) {
        if (lane[i] > 0) {
            return 0;
        }
    }

    return 1;

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

int main(int argc, char** argv) {

    int board_length = 6;
    int starting_seeds = 3;

    // Initialize a game of mancala.
    GameBoard *board = GameBoard_create(board_length, starting_seeds);
    if (board == NULL) {
        return 1;
    }

    // Run main game loop.
    GameBoard_print(board);
    printf("\n");

    GameBoard_play_turn(board, 5);
    GameBoard_print(board);
    printf("\n");

    GameBoard_play_turn(board, 5);
    GameBoard_print(board);
    printf("\n");

    GameBoard_play_turn(board, 2);
    GameBoard_print(board);
    printf("\n");

    // Clean up and exit.
    GameBoard_delete(board);
    return 0;

}
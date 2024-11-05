#include "mancala.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef ARENA
    #include "arena.h"
    Arena *arena;

    void arena_setup() {

        arena = Arena_create(sizeof(GameBoard), 10000);

    }

    void arena_teardown() {

        Arena_delete(arena);

    }

#endif

// Provides an arena allocator for gameboards.
static inline GameBoard *_GameBoard_malloc() {

#ifdef ARENA
    return Arena_allocate(arena);
#else
    return malloc(sizeof(GameBoard));
#endif

}

static inline void _GameBoard_free(GameBoard *board) {

#ifdef ARENA
#else
    return free(board);
#endif

}

GameBoard *GameBoard_create(int length, int starting_seeds) {

    GameBoard *board = _GameBoard_malloc();

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

    board->turn = 0;

    board->play_made.pit_played = -1;
    board->play_made.turn = -1;
    board->play_made.was_capture = -1;
    board->play_made.was_chain = -1;

    return board;

}

GameBoard *GameBoard_copy(GameBoard *board) {

    GameBoard *new_board = GameBoard_create(board->length, 0);

    // Copy the stores.
    new_board->stores[0] = board->stores[0];
    new_board->stores[1] = board->stores[1];

    // Copy the turn.
    new_board->turn = board->turn;

    // Copy the lanes.
    for (int i = 0; i < board->length; i++) {

        new_board->lanes[0][i] = board->lanes[0][i];
        new_board->lanes[1][i] = board->lanes[1][i];

    }

    // Copy the play made.
    new_board->play_made = board->play_made;

    return new_board;

}

void GameBoard_delete(GameBoard *board) {

    free(board->lanes[0]);
    free(board->lanes[1]);

    _GameBoard_free(board);

}

void GameBoard_print(GameBoard *board) {

    // Print the previous turn.
    if (board->play_made.turn != -1) {
        printf("Player %d played pit %d", board->play_made.turn, board->play_made.pit_played);
    }

    if (board->play_made.was_capture == 1) {
        printf(" and captured a piece!");
    }

    if (board->play_made.was_chain == 1) {
        printf(" and gets to play again!");
    }

    printf("\n");

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

        int winner = GameBoard_winner_is(board);

        if (winner == -1) {

            printf("\t Tie.\n");

        } else {

            printf("\t%d won.\n", winner);

        }

    } else {
        printf("\t%d to play.\n", board->turn);
    }

}

int GameBoard_play_turn(GameBoard *board, int pit_to_play) {

    // Constraints:
    //  * pit_to_play < board->length
    //  * board->lane_(board->turn) > 0

    // Record this play.
    board->play_made.pit_played = pit_to_play;
    board->play_made.turn = board->turn;
    board->play_made.was_capture = 0;
    board->play_made.was_chain = 0;

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
        board->play_made.was_chain = 1;
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

            board->play_made.was_capture = 1;

        }

    }

    return board->turn;

}

int GameBoard_is_valid_play(GameBoard *board, int pit_to_play) {

    // Check that the pit is a valid index.
    int valid_index = pit_to_play >= 0 && pit_to_play < board->length;

    // Check that the pit has a non-zero number of seeds.
    int valid_pit = board->lanes[board->turn][pit_to_play] > 0;

    return valid_index && valid_pit;

}

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

int GameBoard_winner_is(GameBoard *board) {

    int score_0 =  GameBoard_score_of(board, 0);
    int score_1 =  GameBoard_score_of(board, 1);

    if (score_0 > score_1) {
        // Player 0 won.
        return 0;
    }

    if (score_0 == score_1) {
        // Tie.
        return -1;
    }

    // Player 1 won.
    return 1;

}

int GameBoard_score_of(GameBoard *board, int player) {

    int score = board->stores[player];
    for (int i = 0; i < board->length; i++) {
        score += board->lanes[player][i];
    }

    return score;

}

int GameBoard_get_successors(GameBoard *board, GameBoard ***successors) {

    // First, find which pits we can play from.
    int *playing_lane = board->lanes[board->turn];

    int number_valid_pits = 0;
    int valid_pits[board->length];
    for (int i = 0; i < board->length; i++) {
        int valid = playing_lane[i] > 0;

        number_valid_pits += valid;
        valid_pits[i] = valid;
    }

    // Create a copy of this board and a successor for each possible play.
    *successors = malloc(sizeof(GameBoard *) * number_valid_pits);

    for (int i = 0, current_play = 0; i < board->length; i++) {
        if (!valid_pits[i]) {
            continue;
        }

        (*successors)[current_play] = GameBoard_copy(board);

        // Play the turn;
        GameBoard_play_turn((*successors)[current_play], i);

        current_play++;

    }

    return number_valid_pits;

}

int GameBoard_utility(GameBoard *board, int for_player) {

    // If the game is over, this is the best (or worst) possible move.
    if (GameBoard_is_game_over(board)) {

        int winner = GameBoard_winner_is(board);

        // Tie.
        if (winner == -1) {
            return 0;
        }

        if (winner == for_player) {
            return INT_MAX;
        }

        // Loser.
        return INT_MIN;
    }

    // We will calculate many different heuristics and take a weighted sum of them.
    int score_advantage = board->stores[for_player] - board->stores[(for_player + 1) % 2];

    return score_advantage;

}

int GameBoard_current_turn(GameBoard *board) {
    return board->turn;
}

int GameBoard_is_dead_state(GameBoard *board, int for_player) {

    int seeds_left = 0;
    for (int i = 0; i < board->length; i++) {
        seeds_left += board->lanes[0][i];
        seeds_left += board->lanes[1][i];
    }

    int possible_score = board->stores[for_player] + seeds_left;
    return possible_score < board->stores[(for_player + 1) % 2];

}
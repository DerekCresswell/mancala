#include "mancala.h"

#include <stdio.h>
#include <stdlib.h>

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

    return new_board;

}

void GameBoard_delete(GameBoard *board) {

    free(board->lanes[0]);
    free(board->lanes[1]);
    free(board);

}

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
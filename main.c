
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mancala.h"
#include "gametree.h"

void run_pvp_console_game(int board_length, int starting_seeds) {

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

void run_pvc_console_game(int board_length, int starting_seeds) {

    // Initialize a game of mancala.
    GameBoard *board = GameBoard_create(board_length, starting_seeds);
    if (board == NULL) {
        return;
    }

    // Initialize our search tree;
    MinMaxSearch search;
    search.depth = 3;

    search.utility = (int (*) (void *state)) &GameBoard_utility;
    search.is_terminal = (int (*) (void *state)) &GameBoard_is_game_over;
    search.get_turn = (int (*) (void *state)) &GameBoard_current_turn;
    search.get_successors = (int (*) (void *state, void ***successors)) &GameBoard_get_successors;
    search.free_state = (void (*) (void *state)) &GameBoard_delete;

    // Run main game loop.
    GameBoard_print(board);
    printf("\n");

    while (!GameBoard_is_game_over(board)) {

        int current_turn = board->turn;
        printf("Player %d's turn: ", current_turn);

        if (current_turn == 0) {

            int pit_to_play = -1;
            scanf("%d", &pit_to_play);
            while (!GameBoard_is_valid_play(board, pit_to_play)) {
                printf("Invalid play. Try again: ");
                scanf("%d", &pit_to_play);
            }

            GameBoard_play_turn(board, pit_to_play);

        } else {

            MinMaxSearch_reset_stats(&search);

            GameBoard *old_board = board;

            Node root;
            root.game_state = board;

            Node *to_play = MinMaxSearch_search(&search, &root);
            MinMaxSearch_print_stats(&search);

            // Swap to our new state.
            board = to_play->game_state;
            to_play->game_state = NULL;

            // Clean up our search.
            Node_cleanup(&root, search.free_state);

        }

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

    run_pvc_console_game(board_length, starting_seeds);

    return 0;

}
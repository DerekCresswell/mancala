
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mancala.h"

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
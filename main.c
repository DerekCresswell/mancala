
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mancala.h"
#include "gametree.h"

typedef int (*player_function) (void *);

int human_player(GameBoard *board) {

    int pit_to_play = -1;
    scanf("%d", &pit_to_play);
    while (!GameBoard_is_valid_play(board, pit_to_play)) {
        printf("Invalid play. Try again: ");
        scanf("%d", &pit_to_play);
    }

    return pit_to_play;

}

int first_player(GameBoard *board) {

    for (int i = 0; i < board->length; i++) {

        if (GameBoard_is_valid_play(board, i)) {
            return i;
        }

    }

    // Should not reach.
    return -1;

}

int last_player(GameBoard *board) {

    for (int i = board->length - 1; i >= 0; i--) {

        if (GameBoard_is_valid_play(board, i)) {
            return i;
        }

    }

    // Should not reach.
    return -1;

}

int random_player(GameBoard *board) {

    // Find all valid plays
    int all_plays[board->length];
    memset(all_plays, 0, board->length * sizeof(int));

    int valid_plays = 0;
    for (int i = 0; i < board->length; i++) {

        if (board->lanes[board->turn] > 0) {
            all_plays[valid_plays] = i;
            valid_plays++;
        }

    }

    int to_play = random() % valid_plays;
    return all_plays[to_play];

}

int minmax_player(GameBoard *board) {

    // Initialize our search tree;
    MinMaxSearch search;
    search.depth = 10;

    search.utility = (int (*) (void *, int)) &GameBoard_utility;
    search.is_terminal = (int (*) (void *)) &GameBoard_is_game_over;
    search.get_turn = (int (*) (void *)) &GameBoard_current_turn;
    search.get_successors = (int (*) (void *, void ***)) &GameBoard_get_successors;
    search.free_state = (void (*) (void *)) &GameBoard_delete;

    MinMaxSearch_reset_stats(&search);

    Node root;
    root.game_state = board;
    root.number_successors = -1;

    Node *to_play = MinMaxSearch_search(&search, &root);
    MinMaxSearch_print_stats(&search);

    // Extract the last turn.
    int pit_to_play = ((GameBoard *) to_play->game_state)->play_made.pit_played;

    // Clean up our search.
    root.game_state = NULL;
    Node_cleanup(&root, search.free_state);

    return pit_to_play;

}

void run_console_game(int board_length, int starting_seeds, player_function player_0, player_function player_1) {

    #ifdef ARENA
        arena_setup();
    #endif

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

        int pit_to_play;
        if (current_turn == 0) {
            pit_to_play = player_0(board);
        } else {
            pit_to_play = player_1(board);
        }

        printf("\n");
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

    #ifdef ARENA
        arena_teardown();
    #endif

}

int main(int argc, char** argv) {

    int board_length = 6;
    int starting_seeds = 3;

    // run_console_game(board_length, starting_seeds, &human_player, &random_player);
    run_console_game(board_length, starting_seeds, &minmax_player, &random_player);
    // run_console_game(board_length, starting_seeds, &human_player, &minmax_player);
    // run_console_game(board_length, starting_seeds, &first_player, &minmax_player);
    // run_console_game(board_length, starting_seeds, &last_player, &minmax_player);

    return 0;

}
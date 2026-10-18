#include "game.h"
#include <stdlib.h>
#include <stddef.h>

/*
Takes as input a pointer to a connect4 game board, a player (0/1), and a int column.
Places a new piece in `col` on the game board.
*/
// Returns 0 on success, -1 if the move is invalid (wrong turn, bad column, or full column).
int make_move(struct game* g, int player, int col) {
    if (player != g->player || col < 0 || col >= BOARD_COLS) {
        return -1;
    }
    // Find the lowest empty row in the specified column.
    for (int r = BOARD_ROWS - 1; r >= 0; r--) {
        if (g->board.mat[r][col] == EMPTY_CELL) {
            g->board.mat[r][col] = player;
            // set to next player's turn
            g->player ^= 1;
            return 0;
        }
    }
    return -1;
}
 
/*
Takes as input a pointer to a connect4 game board.
Returns 0 or 1 if player 0 or 1 has won respectively.
Returns -1 if there is no winner.
*/
int check_win(struct game* g) {
   
}
 
/*
Initializes an existing game with an empty board.
*/
void new_game(struct game* g, int gameId) {
    g->gameId = gameId;
    g->player = 0;
    for (int r = 0; r < BOARD_ROWS; r++) {
        for (int c = 0; c < BOARD_COLS; c++) {
            // Initialize each cell to be empty as -1.
            g->board.mat[r][c] = EMPTY_CELL;
        }
    }
}



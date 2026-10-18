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
    // Directions to check for a win: horizontal, vertical, diagonal down-right, diagonal down-left.
    int dirs[4][2] = {
        {0, 1},   // Horizontal
        {1, 0},   // Vertical
        {1, 1},   // Diagonal down-right
        {1, -1}   // Diagonal down-left
    };

    for (int r = 0; r < BOARD_ROWS; r++) {
        for (int c = 0; c < BOARD_COLS; c++) {
            // Get the player at the current cell.
            int player = g->board.mat[r][c];
            // Skip empty cells.
            if (player == EMPTY_CELL) {
                continue;
            }
            // Check all four directions for a possible win.
            for (int d = 0; d < 4; d++) {
                // Count the current cell as the first in the sequence.
                int count = 1; 

                // Get the row and column increments for the current direction.
                int dr = dirs[d][0];
                int dc = dirs[d][1];

                // Initialize the next row and column to check in the current direction.
                int nr = r + dr;
                int nc = c + dc;

                // Continue checking in the current direction until the sequence ends or a win is found.
                while (nr >= 0 && nr < BOARD_ROWS && nc >= 0 && nc < BOARD_COLS && g->board.mat[nr][nc] == player) {
                    count++;
                    if (count == 4) {
                        // return when a winning sequence of 4 is found for the current player.
                        return player;
                    }
                    // Move to the next cell in the current direction.
                    nr += dr;
                    nc += dc;
                }
            }
        }
    }
    return -1;
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



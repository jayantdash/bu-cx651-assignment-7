#include <arpa/inet.h>
#include <sys/socket.h>


#ifndef COMMON_H
#define COMMON_H

// Define any structs you may find helpful here.

#define BOARD_COLS 7
#define BOARD_ROWS 6
#define EMPTY_CELL -1

// matrix[row][col]; row 0 is the top row. Cells hold EMPTY_CELL or the player (0/1).
struct board {
    int mat[BOARD_ROWS][BOARD_COLS];
};

struct game {
    int gameId;
    int player; // player (0/1) whose move is next
    struct board board;
};

#endif
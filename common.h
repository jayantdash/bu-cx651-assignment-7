#include <arpa/inet.h>
#include <sys/socket.h>


#ifndef COMMON_H
#define COMMON_H

// Define any structs you may find helpful here.

#define BOARD_COLS 7
#define BOARD_ROWS 6
#define EMPTY_CELL -1

#define MAX_GAMES 100  /* must match the malloc in server.c main() */
    
// matrix[row][col]; row 0 is the top row. Cells hold EMPTY_CELL or the player (0/1).
struct board {
    int mat[BOARD_ROWS][BOARD_COLS];
};

struct game {
    int gameId;
    int player; // player (0/1) whose move is next
    struct board board;
};

// Represents the database of all games on the server.
struct gameDB {
    int count;
    struct game games[MAX_GAMES];
    struct sockaddr_in players[MAX_GAMES][2];
    int player_count[MAX_GAMES];
};

enum request_type {
    REQ_CREATE = 1,
    REQ_JOIN,
    REQ_MOVE
};

enum response_status {
    STATUS_OK = 0,
    STATUS_WIN,
    STATUS_LOSE,
    STATUS_TIE,
    STATUS_ERROR
};

struct request {
    enum request_type type;
    int gameId;
    int col;
};

struct response {
    enum response_status status;
    int gameId;
    int player;
    int ready;
    int next_player;
    struct board board;
};

#endif
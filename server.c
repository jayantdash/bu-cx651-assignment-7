#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "game.h"
#include "save.h"

#define PORT 8080
#define SAVE_FILE "games_db.db"

int main(int argc, char* argv[]) {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    // Check if socket creation was successful
    if (sock_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sock_fd);
        return 1;
    }

    printf("Message board server listening on port %d\n", PORT);
    struct gameDB game_db = {0};

    // if the server is started in recovery mode, check for any the save game file and load them into memory.
    // To start the server in recovery mode, populate the argv[1] field as follows: ./server 1
    if (argc > 1 && atoi(argv[1]) == 1) {
        loadgame(&game_db, SAVE_FILE);
    }

    while (1) {
        struct request req = {0};
        struct response res = {0};
        
        socklen_t len = sizeof(client_addr);
        ssize_t n = recvfrom(sock_fd, &req, sizeof(req), 0,(struct sockaddr *)&client_addr, &len);
        if (n != sizeof(req)) {
            continue;
        }

        res.status = STATUS_OK;
        if (req.type == REQ_CREATE) {
            if (game_db.count >= MAX_GAMES) {
                res.status = STATUS_ERROR;
            } else {
                int id = game_db.count++;
                new_game(&game_db.games[id], id);
                game_db.players[id][0] = client_addr;
                game_db.player_count[id] = 1; // set the player count to 1 for the new game
                res.gameId = id;
                res.player = 0;
                res.board = game_db.games[id].board;
                savegame(&game_db, SAVE_FILE);
            }

            sendto(sock_fd, &res, sizeof(res), 0, (struct sockaddr *)&client_addr, len);
        } else if (req.type == REQ_JOIN) {
            int id = req.gameId;
            if (id < 0 || id >= game_db.count || game_db.player_count[id] != 1) {
                res.status = STATUS_ERROR;
                sendto(sock_fd, &res, sizeof(res), 0, (struct sockaddr *)&client_addr, len);
                continue;
            }

            game_db.players[id][1] = client_addr;
            game_db.player_count[id] = 2; // set the player count to 2 when the second player joins
            res.gameId = id;
            res.board = game_db.games[id].board;
            res.ready = 1; // indicate that the game is ready to start
            res.next_player = game_db.games[id].player;
            savegame(&game_db, SAVE_FILE);

            for (int p = 0; p < 2; p++) {
                res.player = p;
                sendto(sock_fd, &res, sizeof(res), 0, (struct sockaddr *)&game_db.players[id][p], sizeof(game_db.players[id][p]));
            }
        } else if (req.type == REQ_MOVE) {
            int id = req.gameId, p = -1;

            if (id < 0 || id >= game_db.count || game_db.player_count[id] != 2 ) {
                continue;
            }

            for (int i = 0; i < 2; i++) {
                if (game_db.players[id][i].sin_addr.s_addr == client_addr.sin_addr.s_addr && game_db.players[id][i].sin_port == client_addr.sin_port) {
                    p = i;
                    break;
                }
            }

            if (p == -1) {
                res.status = STATUS_ERROR;
                sendto(sock_fd, &res, sizeof(res), 0, (struct sockaddr *)&client_addr, len);
                continue;
            }

            if (make_move(&game_db.games[id], p, req.col) != 0) {
                continue;
            }

            res.gameId = id;
            res.board = game_db.games[id].board;
            int winner = check_win(&game_db.games[id]); // check if there's a winner
            int full = 1; // assume the board is full initially

            for (int r = 0; r < BOARD_ROWS; r++)
                for (int c = 0; c < BOARD_COLS; c++)
                    if (game_db.games[id].board.mat[r][c] == EMPTY_CELL) {
                        full = 0; // found an empty cell, board is not full
                    }
            res.ready = 1;
            res.next_player = game_db.games[id].player;
            savegame(&game_db, SAVE_FILE);
            
            for (int i = 0; i < 2; i++) {
                res.player = i;
                if (winner == i) res.status = STATUS_WIN;
                else if (winner != -1) res.status = STATUS_LOSE;
                else if (full) res.status = STATUS_TIE;
                else res.status = STATUS_OK;
                sendto(sock_fd, &res, sizeof(res), 0, (struct sockaddr *)&game_db.players[id][i], sizeof(game_db.players[id][i]));
            }
        }
    }
    close(sock_fd);

    return 0;
}
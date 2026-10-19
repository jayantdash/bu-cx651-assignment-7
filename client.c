#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "common.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096


// prints the 7x6 flattened board array in the correct format
void print_board(int board[]) {
 
    printf("\n");
    for (int col = 0; col < 7; col++)
        printf(" %d", col);
    printf("\n");
 
    for (int row = 0; row < 6; row++) {
        printf("|");
        for (int col = 0; col < 7; col++) {
            int cell = board[row * 7 + col];
            printf("%c|", cell == 0 ? 'X' : cell == 1 ? 'O' : '.');
        }
        printf("\n");
    }
    printf("\n");
}


int main() {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);


    printf("Connect 4\n");
    printf("  1) Create a new game\n");
    printf("  2) Join a game\n");
    printf("> ");
    fflush(stdout);
    int choice = 0;
    scanf("%d", &choice);

    struct request req = {0};
    struct response res = {0};

    if (choice != 1 && choice != 2) { 
        close(sock_fd); 
        return 1; 
    }

    req.type = (choice == 1) ? REQ_CREATE : REQ_JOIN;

    if (req.type == REQ_JOIN) {
        printf("Game ID? >");
        fflush(stdout);
        scanf("%d", &req.gameId);
        printf("\n");
    } else {
        printf("\n");
    }
    // send the initial request to the server
    sendto(sock_fd, &req, sizeof(req), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    socklen_t addr_len = sizeof(server_addr);
    // receive the initial response from the server
    if (recvfrom(sock_fd, &res, sizeof(res), 0, (struct sockaddr *)&server_addr, &addr_len) != sizeof(res)) {
        close(sock_fd); return 1;
    }

    if (res.status == STATUS_ERROR) {
        printf("Unable to create or join game\n");
        close(sock_fd); {
            return 1;
        }    
    }

    int gameId = res.gameId;
    int player = res.player;
    int first = 1;
    while (1) {
        if (first || res.ready) {
            printf("Current Game\n");
            printf("---------------------\n");
            printf("GameID: %d\n", gameId);
            print_board((int *)res.board.mat);
        }
        first = 0;
        if (res.status == STATUS_WIN || res.status == STATUS_LOSE) {
            printf(res.status == STATUS_WIN ? "YOU WIN!\n" : "YOU LOSE!\n");
            break;
        }
        if (res.status == STATUS_ERROR) {
            printf("Invalid request\n");
            break;
        }
        int full = 1;
        // check if the board is full
        for (int i = 0; i < BOARD_ROWS * BOARD_COLS; i++) {
            if (((int *)res.board.mat)[i] == EMPTY_CELL) full = 0;
        }
        // if the board is full, the game is a tie
        if (res.status == STATUS_TIE || full) { 
            printf("TIE\n"); 
            break; 
        }
        
        if (res.ready && res.player == player && (res.next_player == player)) {
            int col;
            printf("\nInput Move Row: > ");
            fflush(stdout);
            if (scanf("%d", &col) != 1) {
                printf("Invalid input\n");
                break;
            }
            printf("\n");
            req.type = REQ_MOVE;
            req.gameId = gameId;
            req.col = col;
            sendto(sock_fd, &req, sizeof(req), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        } else {
            printf("waiting for other player...\n\n");
            fflush(stdout);
        }

        if (recvfrom(sock_fd, &res, sizeof(res), 0, (struct sockaddr *)&server_addr, &addr_len) != sizeof(res)) {
            printf("Failed to receive response from server\n");
            break;
        }
    }

    close(sock_fd);
    return 0;
}
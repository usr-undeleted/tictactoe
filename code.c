#include <stdio.h>
#include <string.h>
#define PLAYER1 0
#define PLAYER2 1

// all the slots, edited by user
char slots[9] = "123456789";

// characters for players
char playerChars[2] = "xo";

// error message edited when handled
// \033[31mPlease input a valid cell!\033[0m
const char *error = "\033[31mPlease input a valid cell!\033[0m";
char errorStr[35] = "";

// scores
const int cells[9] = {
    128,    256,    512,
    16,     32,     64,
    2,      4,      8};

int usedCells[9] = {0};

// print grid
void printGrid() {
    printf("\033[8A%s\n%c | %c | %c\n─────────\n%c | %c | %c\n─────────\n%c | %c | %c\n\n",
    errorStr,
    slots[6], slots[7], slots[8],
    slots[3], slots[4], slots[5],
    slots[0], slots[1], slots[2]);
}

int main () {

    // so that printing a new grid doesnt eat up your prompt
    // the solution of doom #1
    puts("\n\n\n\n\n\n\n");

    // player "scores", basically just the sum of
    // cells defined in cells[]
    int playerScores[2] = {0};
    // 0 is player 1, 1 is player 2

    // current player, always at
    // player 1 by default
    int currentPlr = 1;

    char input;

    for (int i = 0; i < 9; i++) {
        printGrid();

        // if player1 won
        if (playerScores[0] == 546 || playerScores[0] == 168 || // diagonal
            playerScores[0] == 146 || playerScores[0] == 292 || playerScores[0] == 584 || // vertical
            playerScores[0] == 14 || playerScores[0] == 112 || playerScores[0] == 896) { // horizontal
                printf("\033[2K\033[33mPlayer 1 has won!\033[0m\n\n");
                return 1;
        }

        // if player2 won
        if (playerScores[1] == 546 || playerScores[1] == 168 ||
            playerScores[1] == 146 || playerScores[1] == 292 || playerScores[1] == 584 ||
            playerScores[1] == 14 || playerScores[1] == 112 || playerScores[1] == 896) {
                printf("\033[2K\033[33mPlayer 2 has won!\033[0m\n\n");
                return 2;
        }

        // the question
        printf("Player %d, please, pick a valid grid:\033[1C \033[1D", currentPlr);
        fflush(stdin);
        // the solution of doom #2
        scanf(" %c%*", &input);
        while (getchar() != '\n' && getchar() != EOF);

        // check input and use it
        int cnt = 0;
        int j;
        int validity = 0;
        for (j = 1; j < 10; j++) {

            if (usedCells[j - 1] == 1) {
                continue;
            }

            if ((input - '0') == j) {
                slots[j - 1] = playerChars[currentPlr - 1];
                playerScores[currentPlr - 1] += cells[j - 1];
                usedCells[j - 1] = 1;

                validity = 1;

                break;
            } else {
                continue;
            }
        }

        // (in)validity handle
        if (!validity) {
            strcpy(errorStr, error);
            if (i != 0) {
                i--;
            }
            continue;
        }

        // toggle current player
        switch (currentPlr) {
            case 1:
            currentPlr = 2;
            break;
            case 2:
            currentPlr = 1;
            break;
        }

        // set error message to null
        // the solution of doom #3
        strcpy(errorStr, "                                  ");
    }
    printf("\n\033[34mNobody won, what a bummer!\n\n\033[0m");
    return 0;
}

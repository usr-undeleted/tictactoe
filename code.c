#include <stdio.h>
#include <string.h>
#define PLAYER1 0
#define PLAYER2 1

// all the slots, edited by user
char slots[9] = "123456789";

// characters for players
char playerChars[2] = "xo";

// error message edited when handled
const char *error = "\033[31mPlease input a valid cell!\033[0m";
char errorStr[35] = "";

int board[9] = {0};

int usedCells[9] = {0};

// contains the colors for the
// edited slots
char slotColors[9][9] = {
    "", "", "",
    "", "", "",
    "", "", "",
};

// the colors used to edit slotColors
const char *playerColors[2] = {
    "\033[31m", // player 1
    "\033[34m" // player 2
};

const int winCombinations[8][3] = {
    {0,1,2}, {3,4,5}, {6,7,8},
    {0,3,6}, {1,4,7}, {2,5,8},
    {0,4,8}, {2,4,6}
};

int checkWin(int player) {
    for (int w = 0; w < 8; w++) {
        if (board[winCombinations[w][0]] == player &&
            board[winCombinations[w][1]] == player &&
            board[winCombinations[w][2]] == player) {
            return 1;
            }
    }
    return 0;
}

// print grid
void printGrid() {
    printf("\033[8A%s\n%s%c \033[0m| %s%c \033[0m| %s%c\n\033[0m─────────\n%s%c \033[0m| %s%c \033[0m| %s%c\n\033[0m─────────\n%s%c \033[0m| %s%c \033[0m| %s%c\033[0m\n\n",
    errorStr,
    slotColors[6], slots[6], slotColors[7], slots[7], slotColors[8], slots[8],
    slotColors[3], slots[3], slotColors[4], slots[4], slotColors[5], slots[5],
    slotColors[0], slots[0], slotColors[1], slots[1], slotColors[2], slots[2]);
}

int main () {

    // so that printing a new grid doesnt eat up your prompt
    // the solution of doom #1
    puts("\n\n\n\n\n\n\n");

    // current player, always at
    // player 1 by default
    int currentPlr = 1;

    char input;

    for (int i = 0; i < 9; i++) {
        printGrid();

        // if player1 won
        if (checkWin(1)) {
            printf("\033[B");
            printGrid();
            printf("\033[2K\033[33mPlayer 1 has won!\033[0m\n\n");
            return 1;
        }

        // if player2 won
        if (checkWin(2)) {
            printf("\033[B");
            printGrid();
            printf("\033[2K\033[33mPlayer 2 has won!\033[0m\n\n");
            return 2;
        }

        // the question
        printf("Player %d, please, pick a valid grid:\033[1C \033[K\033[1D", currentPlr);
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
                board[j - 1] = currentPlr;
                strcpy(slotColors[j - 1], playerColors[currentPlr - 1]);
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

    // print final result
    printf("\033[B");
    printGrid();
    printf("\n\n\033[34mNobody won, what a bummer!\n\n\033[0m");
    return 0;
}

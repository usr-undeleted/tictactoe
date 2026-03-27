#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

// space to open for grid
#define GRIDSPACE "\n\n\n\n\n\n\n"

char *catFrames[] =  {
  "\033[3A\033[K  A_A \n>('w')<\n (j l)j\n",  // first
  "\033[3A\033[K  A_A \n>(-w-)<\n (j l)j\n",  // blink
  "\033[3A\033[K  A_A \n>('w')<\n (j l)j\n",  // tail 1
  "\033[3A\033[K  A_A \n>('w')<\n (j l)/\n",  // tail 2
  "\033[3A\033[K  A_A \n>('w')<\n (j l)-\n",  // tail 3
  "\033[3A\033[K  A_A \n>('w')<\n (j l)/\n",  // tail 4
  "\033[3A\033[K  A_A \n>('w')<\n (j l)j\n",  // tail 5
  "\033[3A\033[K  A_A .\n>(-w-)<\n (j l)j\n",  // meow 1
  "\033[3A\033[K  A_A .<\n>(-v-)<\n (j l)j\n",  // meow 2
  "\033[3A\033[K  A_A .<meow!>\n>('O')<\n (j l)j\n",  // meow 3
  "\033[3A\033[K  A_A .<meow!>\n>('o')<\n (j l)j\n",  // meow 4
  "\033[3A\033[K  A_A .\n>('w')<\n (j l)j\n",  // meow 5
  NULL
};

void sleep_float(float seconds) {
    struct timespec ts;
    ts.tv_sec = (time_t)seconds;
    ts.tv_nsec = (long)((seconds - ts.tv_sec) * 1e9); // convert decimal to nanoseconds
    nanosleep(&ts, NULL);
}

// pick wether cat is there or not
int catToggle = 0;
// extra newlines added incase
// kitty exists
char *kittyNewLines = "";

// print cat gif
void printCat () {
    // get the amount of frames
    int count = 0;
    while (catFrames[count] != NULL) {
        count++;
    }

    for (int i = 0; i < count; i++) {
        printf("%s", catFrames[i]);
        sleep_float(0.356);
    }
}

// all the slots, edited by user
char slots[9] = "123456789";

// characters for players
char playerChars[2] = "xo";

// error message edited when handled
const char *error = "\033[31mPlease input a valid cell!\033[0m";
char errorStr[35] = "\033[K";

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
    {0,1,2}, {3,4,5}, {6,7,8}, // horizontal
    {0,3,6}, {1,4,7}, {2,5,8}, // vertical
    {0,4,8}, {2,4,6} // diagonal
};

// game mode, 1 = local 2 = vs ai
int gameMode = 1;

// wins per player across rematches
int scores[2] = {0, 0};

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

// reset board state for a new round
void resetGame() {
    memcpy(slots, "123456789", 9);
    memset(board, 0, sizeof(board));
    memset(usedCells, 0, sizeof(usedCells));
    for (int i = 0; i < 9; i++) {
        slotColors[i][0] = '\0';
    }
    strcpy(errorStr, "\033[K");
}

// minimax score for a given board state
int minimax(int depth, int isMaximizing) {
    if (checkWin(2)) return 10 - depth;
    if (checkWin(1)) return depth - 10;

    // check draw
    int isDraw = 1;
    for (int i = 0; i < 9; i++) {
        if (board[i] == 0) { isDraw = 0; break; }
    }
    if (isDraw) return 0;

    if (isMaximizing) {
        int best = -1000;
        for (int i = 0; i < 9; i++) {
            if (board[i] == 0) {
                board[i] = 2;
                int score = minimax(depth + 1, 0);
                board[i] = 0;
                if (score > best) best = score;
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < 9; i++) {
            if (board[i] == 0) {
                board[i] = 1;
                int score = minimax(depth + 1, 1);
                board[i] = 0;
                if (score < best) best = score;
            }
        }
        return best;
    }
}

// pick best move for ai using minimax
int aiMove() {
    int best = -1000;
    int bestMove = -1;
    for (int i = 0; i < 9; i++) {
        if (board[i] == 0) {
            board[i] = 2;
            int score = minimax(0, 0);
            board[i] = 0;
            if (score > best) {
                best = score;
                bestMove = i;
            }
        }
    }
    return bestMove;
}

// name to display for a player
const char *playerName(int player) {
    if (gameMode == 2 && player == 2) return "AI";
    return player == 1 ? "Player 1" : "Player 2";
}

// run one full game, returns winner (1 or 2) or 0 for draw
int playGame() {
    // current player, always at
    // player 1 by default
    int currentPlr = 1;
    int prevPlr = 2;

    char input;

    for (int i = 0; i < 9; i++) {
        printGrid();

        // if player won
        if (checkWin(prevPlr)) {
            printf("\033[B");
            printGrid();
            printf("\033[2K\033[33m%s has won!\033[0m\n\n", playerName(prevPlr));
            return prevPlr;
        }

        // ai turn
        if (gameMode == 2 && currentPlr == 2) {
            printf("AI is thinking...\033[K");
            fflush(stdout);
            sleep_float(0.5);
            printf("\n");
            int move = aiMove();
            slots[move] = playerChars[1];
            board[move] = 2;
            strcpy(slotColors[move], playerColors[1]);
            usedCells[move] = 1;
        } else {
            // the question
            printf("Player %d, please, pick a valid grid:\033[1C \033[K\033[1D", currentPlr);
            // the solution of doom #2
            scanf(" %c%*", &input);
            while (getchar() != '\n' && getchar() != EOF);

            // check input and use it
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
        }

        // toggle current player
        switch (currentPlr) {
            case 1:
            currentPlr = 2;
            prevPlr = 1;
            break;
            case 2:
            currentPlr = 1;
            prevPlr = 2;
            break;
        }

        // set error message to blank
        strcpy(errorStr, "\033[K");
    }

    // check for incase two winning combinations
    // are made. we really dont know why it didnt
    // work in the for loop...
    if (checkWin(prevPlr)) {
        printf("\033[B");
        printGrid();
        printf("\033[2K\033[33m%s has won!\033[0m\033[K\n\n", playerName(prevPlr));
        return prevPlr;
    }

    // print final result
    printf("\033[B");
    printGrid();
    printf("\033[34mNobody won, what a bummer!\033[K\n\n\033[0m");
    return 0;
}

// show start menu, sets gameMode
void showMenu() {
    fputs("=== C TicTacToe! ===\n\n"
        "1. Local multiplayer\n"
        "2. vs AI\n\n"
        "Pick a mode: "
        , stdout);

    char input;
    while (1) {
        scanf(" %c", &input);
        while (getchar() != '\n' && getchar() != EOF);
        if (input == '1' || input == '2') {
            gameMode = input - '0';
            break;
        }
        printf("Please pick 1 or 2: ");
    }
}

// help msg
void helpUsr() {
    fputs("\033[34mThis is free software hosted under https://github.com/usr-undeleted/tictactoe and license under the GPL-3 license.\n"
    "\033[0mUsage: tictactoe [OPTIONS]\n"
    "The program will guide you with a menu.\n\nOptions:\n"
    "    help: Show this message.\n"
    "    cat: Enables kitty animation when someone wins.\n",
    stdout);
    exit(0);
}

int main (int argc, char *argv[]) {

    // get args used
    for (int i = 1; i < argc; i++) {
        // help
        if (!strcmp(argv[i], "help")) {
            helpUsr();
            return 0;
        } else if (!strcmp(argv[i], "cat")) {
            catToggle = 1;
        } else { // invalid arg
            fprintf(stderr, "\033[31mError! Make sure to input a valid argument.\n Use 'tictactoe help' for command usage.\n\033[0m");
            return 10;
        }
    }

    showMenu();

    // so that printing a new grid doesnt eat up your prompt
    // the solution of doom #1
    puts(GRIDSPACE);

    char rematch;
    do {
        resetGame();

        int winner = playGame();

        if (winner != 0) {
            scores[winner - 1]++;
            puts("\n\n");

            if (catToggle) {
                printCat();
            }

            puts("\33[2K\r"); // clear weird err msg that appears

            // proper spacing for cat
            if (!catToggle) {
                puts("\033[5A");
            }
        }

        // show current scores
        printf("\033[33mScore — Player 1: %d | %s: %d\033[0m\n\n",
            scores[0],
            gameMode == 2 ? "AI" : "Player 2",
            scores[1]);

        // ask for rematch
        printf("Play again? (Y/n): ");
        char buf[16];
        if (!fgets(buf, sizeof(buf), stdin)) break;
        rematch = (buf[0] == 'y' || buf[0] == 'Y' || buf[0] == '\n') ? 'y' : 'n';

        if (rematch == 'y' || rematch == 'Y') {
            puts(GRIDSPACE);
        }
    } while (rematch == 'y' || rematch == 'Y');

    return 0;
}

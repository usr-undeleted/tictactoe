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

// get size of double pointer array
int getDoublePointSize(char *array[]) {
    int count = 0;
    while (array[count] != NULL) {
        count++;
    }
    return count;
}

void sleepFloat(float seconds) {
    struct timespec ts;
    ts.tv_sec = (time_t)seconds;
    ts.tv_nsec = (long)((seconds - ts.tv_sec) * 1e9); // convert decimal to nanoseconds
    nanosleep(&ts, NULL);
}

// -- all the options --
//
// pick wether cat is there or not
int catToggle = 0;
//
// remove color if specified
int rmColorToggle = 0;

// print cat gif
void printCat () {
    int count = getDoublePointSize(catFrames);

    for (int i = 0; i < count; i++) {
        printf("%s", catFrames[i]);
        sleepFloat(0.356);
    }
}

// all the slots, edited by user
char slots[9] = "123456789";

// characters for players
char playerChars[2] = "xo";

// error message edited when handled
char *error = "Please input a valid cell!";
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

// general use colors
#define RED     0
#define BLUE    1
#define GREEN   2
#define YELLOW  3
#define PURPLE  4
#define CYAN    5
#define WHITE   6
#define RESET   7
char *ansiColors[] = {
    "\033[31m", // player 1 (red)
    "\033[34m", // player 2 (blue)
    "\033[32m", // green
    "\033[33m", // yellow
    "\033[35m", // purple
    "\033[36m", // cyan
    "\033[37m", // white
    "\033[0m", // reset
    ""
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
    printf("\033[8A%s%s\n%s%c %s| %s%c %s| %s%c\n%s─────────\n%s%c %s| %s%c %s| %s%c\n%s─────────\n%s%c %s| %s%c %s| %s%c%s\n\n",
    ansiColors[RESET] ,errorStr,
    slotColors[6], slots[6], slotColors[7], ansiColors[RESET], slots[7], slotColors[8], ansiColors[RESET], slots[8],
    ansiColors[RESET], slotColors[3], slots[3], slotColors[4], ansiColors[RESET], slots[4], slotColors[5], ansiColors[RESET], slots[5],
    slotColors[0], ansiColors[RESET], slots[0], slotColors[1], ansiColors[RESET], slots[1], slotColors[2], ansiColors[RESET], slots[2], ansiColors[RESET]);
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
    if (gameMode == 2) return "Player";
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
            printf("\033[2K%s%s has won!%s\n\n", ansiColors[YELLOW], playerName(prevPlr), ansiColors[RESET]);
            return prevPlr;
        }

        // ai turn
        if (gameMode == 2 && currentPlr == 2) {
            printf("AI is thinking...\033[K");
            fflush(stdout);
            sleepFloat(0.35);
            printf("\n");
            int move = aiMove();
            slots[move] = playerChars[1];
            board[move] = 2;
            strcpy(slotColors[move], ansiColors[1]);
            usedCells[move] = 1;
        } else {
            // the question
            printf("%s, please, pick a valid grid:\033[1C \033[K\033[1D", playerName(currentPlr));
            char buf[16];
            if (!fgets(buf, sizeof(buf), stdin)) continue;
            input = buf[0];

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
                    strcpy(slotColors[j - 1], ansiColors[currentPlr - 1]);
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
        printf("\033[2K%s%s has won!%s\033[K\n\n",
            ansiColors[YELLOW], playerName(prevPlr),
            ansiColors[RESET]);
        return prevPlr;
    }

    // print final resuchar **arraylt
    printf("\033[B");
    printGrid();
    printf("%sNobody won, what a bummer!\033[K\n\n%s",
        ansiColors[BLUE], ansiColors[RESET]);
    return 0;
}

// show start menu, sets gameMode
void showMenu() {
    printf("%s=== C TicTacToe! ===\n\n"
        "1. Local multiplayer\n"
        "2. vs AI\n\n"
        "Pick a mode: ",
        ansiColors[RESET], stdout);

    char input;
    while (1) {
        char buf[16];
        if (!fgets(buf, sizeof(buf), stdin)) continue;
        input = buf[0];
        if (input == '1' || input == '2') {
            gameMode = input - '0';
            break;
        }
        printf("Please pick 1 or 2: ");
    }
}

// help msg
void helpUsr() {
    printf("%sThis is free software hosted under https://github.com/usr-undeleted/tictactoe and license under the GPL-3 license.\n"
    "%sUsage: tictactoe [OPTIONS]\n"
    "The program will guide you with a menu.\n\nOptions:\n"
    "   help: Show this message.\n"
    "   cat: Enables kitty animation when someone wins.\n"
    "   no-color: Disable all color in code.\n",
    ansiColors[BLUE], ansiColors[RESET],
    stdout);
    exit(0);
}

int main (int argc, char *argv[]) {

    // get args used
    for (int i = 1; i < argc; i++) {
        // help
        if (!strcmp(argv[i], "no-color")) {
            rmColorToggle = 1;

        } else if (!strcmp(argv[i], "help")) {
            helpUsr();
            return 0;

        } else if (!strcmp(argv[i], "cat")) {
            catToggle = 1;

        } else { // invalid arg
            fprintf(stderr, "%sError! Make sure to input a valid argument.\n Use 'tictactoe help' for command usage.\n%s", ansiColors[0], ansiColors[7]);
            return 10;
        }
    }

    // remove colors
    // 9 since it is all ansi colors
    if (rmColorToggle) {
        int count = getDoublePointSize(ansiColors);
        printf("%s ", ansiColors[0]);
        for (int i = 0; i < count - 2; i++) {
            ansiColors[i] = "";
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
            } else {
                puts("\033[5A");
            }

            puts("\33[2K\r"); // clear weird err msg that appears
        }

        // show current scores
        printf("%sScore — %s: %d | %s: %d%s\n\n",
            ansiColors[YELLOW],
            playerName(1), scores[0],
            playerName(2), scores[1],
            ansiColors[RESET]);

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

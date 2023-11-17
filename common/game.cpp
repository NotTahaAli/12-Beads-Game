#include "game.h"
// 0 means Empty, 1 Means Red, -1 Means Blue
int board[5][5] = {{0}};

// Coordinates of Last played Turn (-1,-1) means not counting.
Coordinates lastTurn = {
    -1, // X Coordinate
    -1, // Y Coordinate
};

// 1 Means Red, -1 Means Blue
int turn = -1;

// 1 Means Red, -1 Means Blue
int getTurn() {
    return turn;
}

void resetLastTurn() {
    lastTurn = {-1,-1};
}

void nextTurn() {
    turn *= -1;
    resetLastTurn();
}

void initGame() {
    turn = -1;
    resetLastTurn();
    for (int i = 0; i < 25; i++) {
        board[i%5][i/5] = ((i < 12) ? 1 : ((i > 12) ? -1 : 0));
    }
}

// 0 means game is continuing, 1 means Red won, -1 means Blue won
int checkVictory() {
    bool blueFound = false;
    bool redFound = false;
    for (int i = 0; i < 25; i++) {
        if (board[i%5][i/5] == -1) blueFound = true;
        else if (board[i%5][i/5] == 1) redFound = true;
        if (blueFound && redFound) return 0;
    }
    return ((blueFound) ? -1 : 1);
}

// 0 means Empty, 1 Means Red, -1 Means Blue
int getValueAtPosition(int x, int y) {
    if (x < 0 || x >= 5) return -2; // Error X not in Range;
    if (y < 0 || y >= 5) return -3; // Error Y not in Range;
    return board[x][y];
}

// 0 means no move, 1 means single move, 2 means take and move 2 steps.
// Moves Index is in the following directions. (X marking the current position)
//   0 1 2
// 0 O O O
// 1 O X O
// 2 O O O
void getMoves(int x, int y, int moves[3][3]) {
    int color = getValueAtPosition(x,y);
    int colorAtPos;
    int i,j;
    bool killOnly = (lastTurn.x != -1);
    if (turn != color || (killOnly && (lastTurn.x != x || lastTurn.y != y))) {
        for (i = 0; i < 9; i++) {
            moves[i/3][i%3] = 0;
        }
        return;
    };
    int move;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if ((y*5+x)%2 == 1 && i*j != 0) move = 0;
            else {
                colorAtPos = getValueAtPosition(x+j,y+i);
                move = 0;
                if (colorAtPos == 0) move = ((killOnly) ? 0 : 1);
                else if (colorAtPos == -color && getValueAtPosition(x+(2*j),y+(2*i)) == 0) move = 2;
            }
            moves[j+1][i+1] = move;
        }
    }
}

void executeMotion(Motion move) {
    if (move.to.x >= 0 && move.to.y < 5) {
        board[move.to.x][move.to.y] = board[move.from.x][move.from.y];
    }
    board[move.from.x][move.from.y] = 0;
}

// 0 is Turn not played
// 1 is Turn played current person will continue.
// 2 is Turn played other person will play next turn
// 3 is Game Over. Current player won.
// x2 and y2 are the indexes from getMoves.
int playTurn(int x, int y, int x2, int y2) {
    if (x2 < 0 || x2 >= 3 || y2 < 0 || y2 >= 3) return 0;
    int possibleMoves[3][3];
    getMoves(x,y,possibleMoves);
    if (possibleMoves[x2][y2] == 0) return 0;
    if (possibleMoves[x2][y2] == 1) {
        executeMotion({{x,y},{x+x2-1,y+y2-1}});
        nextTurn();
        return 2;
    }
    x2--;
    y2--;
    executeMotion({{x,y},{x+(2*x2),y+(2*y2)}});
    executeMotion({{x+x2,y+y2},{-1,-1}});
    x += 2*x2;
    y += 2*y2;
    lastTurn = {x,y};
    getMoves(x,y,possibleMoves);
    for (int i = 0; i < 9; i++) {
        if (possibleMoves[i%3][i/3] == 2) {
            if (checkVictory() == 0) return 1;
            turn = 0;
            return 3;
        }
    }
    nextTurn();
    return 2;
}
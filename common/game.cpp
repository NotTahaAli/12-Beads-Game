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
int getValueAtPosition(Coordinates pos) {
    if (pos.x < 0 || pos.x >= 5) return -2; // Error X not in Range;
    if (pos.y < 0 || pos.y >= 5) return -3; // Error Y not in Range;
    return board[pos.x][pos.y];
}

// 0 means no move, 1 means single move, 2 means take and move 2 steps.
// Moves Index is in the following directions. (X marking the current position)
//   0 1 2
// 0 O O O
// 1 O X O
// 2 O O O
void getMoves(Coordinates pos, int moves[3][3]) {
    int color = getValueAtPosition(pos);
    int colorAtPos;
    int i,j;
    bool killOnly = (lastTurn.x != -1);
    if (turn != color || (killOnly && (lastTurn.x != pos.x || lastTurn.y != pos.y))) {
        for (i = 0; i < 9; i++) {
            moves[i/3][i%3] = 0;
        }
        return;
    };
    int move;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if ((pos.y*5+pos.x)%2 == 1 && i*j != 0) move = 0;
            else {
                colorAtPos = getValueAtPosition({pos.x+j,pos.y+i});
                move = 0;
                if (colorAtPos == 0) move = ((killOnly) ? 0 : 1);
                else if (colorAtPos == -color && getValueAtPosition({pos.x+(2*j),pos.y+(2*i)}) == 0) move = 2;
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
// move is coordinate form of indexes from getMoves.
int playTurn(Coordinates pos, Coordinates move) {
    if (move.x < 0 || move.x >= 3 || move.y < 0 || move.y >= 3) return 0;
    int possibleMoves[3][3];
    getMoves(pos,possibleMoves);
    if (possibleMoves[move.x][move.y] == 0) return 0;
    if (possibleMoves[move.x][move.y] == 1) {
        executeMotion({pos,{pos.x+move.x-1,pos.y+move.y-1}});
        nextTurn();
        return 2;
    }
    move.x--;
    move.y--;
    executeMotion({pos,{pos.x+(2*move.x),pos.y+(2*move.y)}});
    executeMotion({{pos.x+move.x,pos.y+move.y},{-1,-1}});
    pos.x += 2*move.x;
    pos.y += 2*move.y;
    lastTurn = pos;
    getMoves(pos,possibleMoves);
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
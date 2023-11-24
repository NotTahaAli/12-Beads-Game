#include "game.h"
#include <fstream>

// 1 Means Red, -1 Means Blue
int getTurn(gameState &game) {
    return game.turn;
}

void resetLastTurn(gameState &game) {
    game.lastTurn = {-1,-1};
}

void nextTurn(gameState &game) {
    game.turn *= -1;
    resetLastTurn(game);
}

void saveGameState(gameState game) {
    std::ofstream outFile("./data/game.dat");
    if (!game.gameOver) {
        outFile.write((char *) &game, sizeof(gameState));
    } else {
        outFile.write("",0);
    }
    outFile.close();
}

gameState initGame(bool loadPrevious) {
    gameState game;
    if (loadPrevious) {
        std::ifstream inFile("./data/game.dat");
        if (inFile.is_open() && !inFile.eof()) {
            inFile.read((char *) &game, sizeof(gameState));
        }
        inFile.close();
    }
    if (game.gameOver) {
        game.gameOver = false;
        for (int i = 0; i < 25; i++) {
            // 0 means Empty, 1 Means Red, -1 Means Blue
            game.board[i%5][i/5] = ((i < 12) ? 1 : ((i > 12) ? -1 : 0));
        }
        srand(time(0));
        game.turn = (rand() % 2) ? 1 : -1;
    }
    return game;
}

// 0 means game is continuing, 1 means Red won, -1 means Blue won
int checkVictory(gameState &game) {
    bool blueFound = false;
    bool redFound = false;
    for (int i = 0; i < 25; i++) {
        if (game.board[i%5][i/5] == -1) blueFound = true;
        else if (game.board[i%5][i/5] == 1) redFound = true;
        if (blueFound && redFound) return 0;
    }
    return ((blueFound) ? -1 : 1);
}

// 0 means Empty, 1 Means Red, -1 Means Blue
int getValueAtPosition(gameState &game, Coordinates pos) {
    if (pos.x < 0 || pos.x >= 5) return -2; // Error X not in Range;
    if (pos.y < 0 || pos.y >= 5) return -3; // Error Y not in Range;
    return game.board[pos.x][pos.y];
}

// 0 means no move, 1 means single move, 2 means take and move 2 steps.
// Moves Index is in the following directions. (X marking the current position)
//   0 1 2
// 0 O O O
// 1 O X O
// 2 O O O
void getMoves(gameState &game, Coordinates pos, int moves[3][3]) {
    int color = getValueAtPosition(game,pos);
    int colorAtPos;
    int i,j;
    bool killOnly = (game.lastTurn.x != -1);
    if (game.turn != color || (killOnly && (game.lastTurn.x != pos.x || game.lastTurn.y != pos.y))) {
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
                colorAtPos = getValueAtPosition(game, {pos.x+j,pos.y+i});
                move = 0;
                if (colorAtPos == 0) move = ((killOnly) ? 0 : 1);
                else if (colorAtPos == -color && getValueAtPosition(game, {pos.x+(2*j),pos.y+(2*i)}) == 0) move = 2;
            }
            moves[j+1][i+1] = move;
        }
    }
}

void executeMotion(gameState &game, Motion move) {
    if (move.to.x >= 0 && move.to.y < 5) {
        game.board[move.to.x][move.to.y] = game.board[move.from.x][move.from.y];
    }
    game.board[move.from.x][move.from.y] = 0;
}

// 0 is Turn not played
// 1 is Turn played current person will continue.
// 2 is Turn played other person will play next turn
// 3 is Game Over. Current player won.
// move is coordinate form of indexes from getMoves.
turnData playTurn(gameState &game, Coordinates pos, Coordinates move) {
    if (move.x < 0 || move.x >= 3 || move.y < 0 || move.y >= 3) return {0};
    int possibleMoves[3][3];
    Motion moves[2];
    getMoves(game, pos,possibleMoves);
    if (possibleMoves[move.x][move.y] == 0) return {0};
    if (possibleMoves[move.x][move.y] == 1) {
        moves[0] = {pos,{pos.x+move.x-1,pos.y+move.y-1}};
        moves[1] = {{-1,-1},{-1,-1}};
        executeMotion(game, moves[0]);
        nextTurn(game);
        return {2, moves[0], moves[1]};
    }
    move.x--;
    move.y--;
    moves[0] = {pos,{pos.x+(2*move.x),pos.y+(2*move.y)}};
    moves[1] = {{pos.x+move.x,pos.y+move.y},{-1,-1}};
    executeMotion(game, moves[0]);
    executeMotion(game, moves[1]);
    pos.x += 2*move.x;
    pos.y += 2*move.y;
    game.lastTurn = pos;
    getMoves(game, pos,possibleMoves);
    if (checkVictory(game) != 0) {
        game.turn = 0;
        game.gameOver = true;
        return {3, moves[0], moves[1]};
    }
    for (int i = 0; i < 9; i++) {
        if (possibleMoves[i%3][i/3] == 2) {
            if (checkVictory(game) == 0) return {1, moves[0], moves[1]};;
        }
    }
    nextTurn(game);
    return {2, moves[0], moves[1]};
}
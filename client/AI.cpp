#include "AI.h"
#include <cmath>

const int minPossibleValue = -14;
const int maxPossibleValue = 14;
const int maximisingPlayer = 1;

int evaluate(gameState game) {
    int evaluation = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            evaluation += game.board[i][j];
        }
    }
    return evaluation;
}

// Red Player is Maximising
int minimax(gameState game, int depth, int alpha, int beta) {
    bool maximisingTurn = (game.turn == maximisingPlayer);
    if (depth <= 0) return evaluate(game);
    int moves[5][5][3][3];
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            getMoves(game, {j,i}, moves[j][i]);
        }
    }
    int eval;
    int moveI, moveJ, moveK, moveL;
    bool loop = true;
    int turnStat;
    gameState gameTemp;
    if (maximisingTurn) {
        int maxEval = minPossibleValue;
        for (int i = 0; loop && i < 5; i++) {
            for (int j = 0; loop && j < 5; j++) {
                for (int k = 0; loop && k < 3; k++) {
                    for (int l = 0; l < 3; l++) {
                        if (moves[j][i][l][k] == 0) continue;
                        gameTemp = game;
                        turnStat = playTurn(gameTemp, {j,i}, {l,k}).status;
                        if (turnStat == 0) continue;
                        if (turnStat == 3) return 13;
                        eval = minimax(gameTemp, depth-((turnStat == 1) ? 0: 1), alpha, beta);
                        if (eval > maxEval) {
                            maxEval = eval;
                            moveI = i;
                            moveJ = j;
                            moveK = k;
                            moveL = l;
                            if (maxEval > alpha) {
                                alpha = maxEval;
                            }
                        }
                        if (beta <= alpha) {
                            loop = false;
                            break;
                        }
                    }
                }
            }
        }
        return maxEval;
    } else {
        int minEval = maxPossibleValue;
        for (int i = 0; loop && i < 5; i++) {
            for (int j = 0; loop && j < 5; j++) {
                for (int k = 0; loop && k < 3; k++) {
                    for (int l = 0; l < 3; l++) {
                        if (moves[j][i][l][k] == 0) continue;
                        gameTemp = game;
                        turnStat = playTurn(gameTemp, {j,i}, {l,k}).status;
                        if (turnStat == 0) continue;
                        if (turnStat == 3) return 13;
                        eval = minimax(gameTemp, depth-((turnStat == 1) ? 0: 1), alpha, beta);
                        if (eval < minEval) {
                            minEval = eval;
                            moveI = i;
                            moveJ = j;
                            moveK = k;
                            moveL = l;
                            if (minEval < beta) {
                                beta = minEval;
                            }
                        }
                        if (beta <= alpha) {
                            loop = false;
                            break;
                        }
                    }
                }
            }
        }
        return minEval;
    }
}

Motion minimax(gameState game, int depth) {
    int alpha = minPossibleValue;
    int beta = maxPossibleValue;
    bool maximisingTurn = (game.turn == maximisingPlayer);
    int moves[5][5][3][3];
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            getMoves(game, {j,i}, moves[j][i]);
        }
    }
    int eval;
    int moveI, moveJ, moveK, moveL;
    bool loop = true;
    int turnStat;
    gameState gameTemp;
    if (maximisingTurn) {
        int maxEval = minPossibleValue;
        for (int i = 0; loop && i < 5; i++) {
            for (int j = 0; loop && j < 5; j++) {
                for (int k = 0; loop && k < 3; k++) {
                    for (int l = 0; l < 3; l++) {
                        if (moves[j][i][l][k] == 0) continue;
                        gameTemp = game;
                        turnStat = playTurn(gameTemp, {j,i}, {l,k}).status;
                        if (turnStat == 0) continue;
                        if (turnStat == 3) return {{j,i},{l,k}};
                        eval = minimax(gameTemp, depth-((turnStat == 1) ? 0: 1), alpha, beta);
                        if (eval > maxEval) {
                            maxEval = eval;
                            moveI = i;
                            moveJ = j;
                            moveK = k;
                            moveL = l;
                            if (maxEval > alpha) {
                                alpha = maxEval;
                            }
                        }
                        if (beta <= alpha) {
                            loop = false;
                            break;
                        }
                    }
                }
            }
        }
    } else {
        int minEval = maxPossibleValue;
        for (int i = 0; loop && i < 5; i++) {
            for (int j = 0; loop && j < 5; j++) {
                for (int k = 0; loop && k < 3; k++) {
                    for (int l = 0; l < 3; l++) {
                        if (moves[j][i][l][k] == 0) continue;
                        gameTemp = game;
                        turnStat = playTurn(gameTemp, {j,i}, {l,k}).status;
                        if (turnStat == 0) continue;
                        if (turnStat == 3) return {{j,i},{l,k}};
                        eval = minimax(gameTemp, depth-((turnStat == 1) ? 0: 1), alpha, beta);
                        if (eval < minEval) {
                            minEval = eval;
                            moveI = i;
                            moveJ = j;
                            moveK = k;
                            moveL = l;
                            if (minEval < beta) {
                                beta = minEval;
                            }
                        }
                        if (beta <= alpha) {
                            loop = false;
                            break;
                        }
                    }
                }
            }
        }
    }
    return {{moveJ,moveI},{moveL,moveK}};
}
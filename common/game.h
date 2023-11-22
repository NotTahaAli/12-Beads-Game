struct Coordinates {
    int x;
    int y;
};

// To should be {x: -1, y: -1} to remove.
struct Motion {
    Coordinates from, to;
};

struct turnData {
    // 0 is Turn not played
    // 1 is Turn played current person will continue.
    // 2 is Turn played other person will play next turn
    // 3 is Game Over. Current player won.
    int status;
    Motion move;
    Motion remove;
};

struct gameState {
    // 0 means Empty, 1 Means Red, -1 Means Blue
    int board[5][5] = {{0}};

    // Coordinates of Last played Turn (-1,-1) means not counting.
    Coordinates lastTurn = {
        -1, // X Coordinate
        -1, // Y Coordinate
    };

    // 1 Means Red, -1 Means Blue
    int turn = -1;
};

int getTurn(gameState &game);
gameState initGame();
int checkVictory(gameState &game);
int getValueAtPosition(gameState &game, Coordinates pos);
void getMoves(gameState &game, Coordinates pos, int moves[3][3]);
turnData playTurn(gameState &game, Coordinates pos, Coordinates move);
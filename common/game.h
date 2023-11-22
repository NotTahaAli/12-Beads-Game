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

int getTurn();
void initGame();
int checkVictory();
int getValueAtPosition(Coordinates pos);
void getMoves(Coordinates pos, int moves[3][3]);
turnData playTurn(Coordinates pos, Coordinates move);
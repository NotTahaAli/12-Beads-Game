struct Coordinates {
    int x;
    int y;
};

// To should be {x: -1, y: -1} to remove.
struct Motion {
    Coordinates from, to;
};

int getTurn();
void initGame();
int checkVictory();
int getValueAtPosition(int x, int y);
void getMoves(int x, int y, int moves[3][3]);
int playTurn(int x, int y, int x2, int y2);
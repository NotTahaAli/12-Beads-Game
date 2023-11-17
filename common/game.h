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
int getValueAtPosition(Coordinates pos);
void getMoves(Coordinates pos, int moves[3][3]);
int playTurn(Coordinates pos, Coordinates move);
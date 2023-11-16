#include "server/game.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

void clearScreen() {
    #if __WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause() {
    #if __WIN32
        system("pause");
    #else
        system("read -p \"Press enter to continue...\" s");
    #endif
}

int main()
{
    initGame();
    int turn;
    int x, y, moves[3][3], x2, y2;
    bool movesPossible = false;
    int turnPlayed = 0;
    while (turnPlayed != 3)
    {
        while (true)
        {
            clearScreen();
            cout << "    0 1 2 3 4" << endl;
            cout << "--------------" << endl;
            for (int i = 0; i < 5; i++)
            {
                cout << i << " | ";
                for (int j = 0; j < 5; j++)
                {
                    int val = getValueAtPosition(j, i);
                    cout << ((val == 0) ? " " : ((val == 1) ? "X" : "O")) << " ";
                }
                cout << endl;
            }
            turn = getTurn();
            cout << "Current Turn: " << ((turn == 0) ? "Game Ended" : ((turn == 1) ? "X" : "O")) << endl;
            while (true)
            {
                cout << "Enter Coordinates of Bead: ";
                cin >> x >> y;
                if (cin.fail())
                {
                    cin.clear();
                    fflush(stdin);
                }
                else
                    break;
            }
            cout << "Moves: " << endl;
            getMoves(x, y, moves);
            movesPossible = false;
            cout << "    0 1 2" << endl;
            cout << "----------" << endl;
            for (int i = 0; i < 3; i++)
            {
                cout << i << " | ";
                for (int j = 0; j < 3; j++)
                {
                    cout << moves[j][i] << " ";
                    if (moves[j][i] != 0)
                        movesPossible = true;
                }
                cout << endl;
            }
            if (!movesPossible)
            {
                cout << "No Moves Possible for this Bead, Try again with another bead." << endl;
                pause();
                continue;
            }
            while (true)
            {
                cout << "Enter Move Coordinates [0-2, 0-2] ([-1 -1] for change bead) : ";
                cin >> x2 >> y2;
                if (cin.fail())
                {
                    cin.clear();
                    fflush(stdin);
                }
                else
                    break;
            }
            if (x2 == -1 && y2 == -1)
                break;
            turnPlayed = playTurn(x, y, x2, y2);
            if (turnPlayed != 0)
            {
                break;
            }
            cout << "Turn could not be played try again." << endl;
            pause();
        };
    }
    cout << "Game Over. " << ((checkVictory() == 1) ? "X" : "O") << " won.";
}
#include <string>
#include "menuHandler.h"
#include "gameHandler.h"

extern Board board;
extern Menu onlineMenu;
extern int currentPlayer;
extern void showPopup(std::string message);
extern void startNewGame();

void setupClient();
void closeClient();

bool connect(std::string const URL);
void closeSocket();

void sendGetGameState();
void sendGetMoves(sf::Vector2i gridPos);
void sendPlayMove(sf::Vector2i gridPos, sf::Vector2i move);
void sendGetCurrentPlayer();
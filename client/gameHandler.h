#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../common/game.h"

extern void showPopup(std::string message);
extern sf::Music backgroundMusic;
extern sf::Sound victorySound, defeatSound;

struct Bead {
    sf::Sprite sprite;
    sf::Vector2f speed;
    sf::Vector2f gridPos;
    sf::Vector2f gridTarget;
};

struct Highlight {
    sf::Sprite sprite;
    sf::Vector2i move = {-1, -1};
    sf::Vector2i base = {-1, -1};
};

struct Board {
    sf::Sprite sprite;
    Bead beads[5][5];
    Highlight highlights[5][5];
    gameState game;
    bool visible = false;
    bool blocked = true;
    bool isOnline = false;
    sf::CircleShape playerBeadHighlight;
};

void moveBead(Board &board, Bead &bead, sf::Vector2i newGridPos, unsigned int frames = 15);
void removeBead(Bead &bead, unsigned int frames = 15);

Board setUpBoard(bool loadPrevious = true);
void drawBoard(sf::RenderWindow &window, Board &board);
void setBoardSize(Board &board, sf::Vector2i SizeInPixels);
void centerBoard(Board &board, sf::RenderWindow &window);
void checkClick(Board &board, sf::Event::MouseButtonEvent mouseButton);
bool checkHover(Board &board, sf::Event::MouseMoveEvent mouseMove);

void resetHighlights(Board &board);
void showHighlight(Board &board, sf::Vector2i gridPos, sf::Vector2i move, int moveVal);
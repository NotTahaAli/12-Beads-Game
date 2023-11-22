#include <SFML/Graphics.hpp>
#include "../common/game.h"

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
};

void moveBead(Bead &bead, sf::Vector2i newGridPos, unsigned int frames = 15);
void removeBead(Bead &bead, unsigned int frames = 15);

Board setUpBoard();
void drawBoard(sf::RenderWindow &window, Board &board);
void setBoardSize(Board &board, sf::Vector2i SizeInPixels);
void centerBoard(Board &board, sf::RenderWindow &window);
void checkClick(Board &board, sf::Event::MouseButtonEvent mouseButton);
void checkHover(sf::RenderWindow &window, Board &board, sf::Event::MouseMoveEvent mouseMove);
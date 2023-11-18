#include <SFML/Graphics.hpp>

struct Bead {
    sf::Sprite sprite;
    sf::Vector2f speed;
    sf::Vector2f gridPos;
    sf::Vector2f gridTarget;
};

struct Board {
    sf::Sprite sprite;
    Bead beads[5][5];
};

void moveBead(Bead &bead, sf::Vector2i newGridPos, unsigned int frames = 30);

Board setUpBoard();
void drawBoard(sf::RenderWindow &window, Board &board);
void setBoardSize(Board &board, sf::Vector2i SizeInPixels);
void centerBoard(Board &board, sf::RenderWindow &window);
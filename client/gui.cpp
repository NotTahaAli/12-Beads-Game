#include "spriteHandler.h"

#include <iostream>
using namespace std;

void setIcon(string fileLocation, sf::RenderWindow &window)
{
    sf::Image icon;
    icon.loadFromFile(fileLocation);
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
}

// Highlight positions
void highlightPosition(sf::RenderWindow &window, sf::Vector2f position, sf::Vector2f size, sf::Color color)
{
    sf::RectangleShape rectangle(size);
    rectangle.setFillColor(color);
    rectangle.setPosition(position);
    window.draw(rectangle);
}

int main()
{
    sf::VideoMode windowMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(windowMode, "12 Beads Game", sf::Style::None);
    window.setFramerateLimit(60);
    setIcon("./assets/redBead.png", window);

    Board board = setUpBoard();
    int minDimension = 0.9 * ((window.getSize().x < window.getSize().y) ? window.getSize().x : window.getSize().y);
    setBoardSize(board, {minDimension, minDimension});
    centerBoard(board, window);

    int possibleMoves[3][3];

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseMoved)
                checkHover(window, board, event.mouseMove);
            else if (event.type == sf::Event::MouseButtonPressed)
                checkClick(board, event.mouseButton);
        }

        window.clear();

        drawBoard(window, board);

        window.display();
    }

    return 0;
}
#include "beads.h"

#include <iostream>
using namespace std;

void setIcon(string fileLocation, sf::RenderWindow &window)
{
    sf::Image icon;
    icon.loadFromFile(fileLocation);
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
}

int main()
{
    sf::VideoMode windowMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(windowMode, "12 Beads Game", sf::Style::None);
    window.setFramerateLimit(30);
    setIcon("./assets/redBead.png", window);

    Board board = setUpBoard();
    int minDimension = 0.9 * ((windowMode.width < windowMode.height) ? windowMode.width : windowMode.height);
    setBoardSize(board, {minDimension, minDimension});
    centerBoard(board, window);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        drawBoard(window, board);

        window.display();
    }

    return 0;
}
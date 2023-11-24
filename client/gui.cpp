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

void fillHeight(sf::RenderWindow &window, sf::Sprite &sprite) {
    sf::Vector2i textureSize = sprite.getTextureRect().getSize();
    sf::Vector2u windowSize = window.getSize();

    sprite.setScale((float) windowSize.y / textureSize.y , (float) windowSize.y / textureSize.y );
}

void fillWidth(sf::RenderWindow &window, sf::Sprite &sprite) {
    sf::Vector2i textureSize = sprite.getTextureRect().getSize();
    sf::Vector2u windowSize = window.getSize();

    sprite.setScale((float) windowSize.x/ textureSize.x , (float) windowSize.x / textureSize.x );
}

void cover(sf::RenderWindow &window, sf::Sprite &sprite) {
    sf::Vector2i textureSize = sprite.getTextureRect().getSize();
    sf::Vector2u windowSize = window.getSize();

    if ((float) textureSize.x / textureSize.y > (float) windowSize.x / windowSize.y) {
        fillHeight(window, sprite);
    } else {
        fillWidth(window, sprite);
    }
}

int main()
{
    sf::VideoMode windowMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(windowMode, "12 Beads Game", sf::Style::None);
    window.setFramerateLimit(60);
    setIcon("./assets/redBead.png", window);

    sf::Texture back;
    back.loadFromFile("./assets/screenBackground.png");
    sf::Texture left;
    left.loadFromFile("./assets/screenLeftDetails.png");
    sf::Texture right;
    right.loadFromFile("./assets/screenRightDetails.png");

    sf::Sprite backSprite;
    backSprite.setTexture(back);
    backSprite.setOrigin(back.getSize().x/2.f, back.getSize().y/2.f);
    backSprite.setPosition(window.getSize().x/2.f, window.getSize().y/2.f);
    cover(window, backSprite);

    sf::Sprite leftSprite;
    leftSprite.setTexture(left);
    fillHeight(window, leftSprite);

    sf::Sprite rightSprite;
    rightSprite.setTexture(right);
    rightSprite.setOrigin(right.getSize().x, 0);
    rightSprite.setPosition(window.getSize().x, 0);
    fillHeight(window, rightSprite);

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
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.control && event.key.code == sf::Keyboard::Q) {
                    window.close();
                }
            }
            else if (event.type == sf::Event::MouseMoved)
                checkHover(window, board, event.mouseMove);
            else if (event.type == sf::Event::MouseButtonPressed)
                checkClick(board, event.mouseButton);
        }

        window.clear();

        window.draw(backSprite);
        window.draw(leftSprite);
        window.draw(rightSprite);
        drawBoard(window, board);

        window.display();
    }

    saveGameState(board.game);

    return 0;
}
#include "gameHandler.h"
#include "menuHandler.h"

#include <iostream>
using namespace std;

Menu mainMenu, pauseMenu;
Board board;

sf::Texture mainMenuButton;
sf::Vector2f mainMenuButtonSize;
sf::Font font;

sf::RenderWindow window;

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

void fillHeight(sf::RenderWindow &window, sf::Sprite &sprite)
{
    sf::Vector2i textureSize = sprite.getTextureRect().getSize();
    sf::Vector2u windowSize = window.getSize();

    sprite.setScale((float)windowSize.y / textureSize.y, (float)windowSize.y / textureSize.y);
}

void fillWidth(sf::RenderWindow &window, sf::Sprite &sprite)
{
    sf::Vector2i textureSize = sprite.getTextureRect().getSize();
    sf::Vector2u windowSize = window.getSize();

    sprite.setScale((float)windowSize.x / textureSize.x, (float)windowSize.x / textureSize.x);
}

void cover(sf::RenderWindow &window, sf::Sprite &sprite)
{
    sf::Vector2i textureSize = sprite.getTextureRect().getSize();
    sf::Vector2u windowSize = window.getSize();

    if ((float)textureSize.x / textureSize.y > (float)windowSize.x / windowSize.y)
    {
        fillHeight(window, sprite);
    }
    else
    {
        fillWidth(window, sprite);
    }
}

void startNewGame() {
    board = setUpBoard(false);
    int minDimension = 0.9 * ((window.getSize().x < window.getSize().y) ? window.getSize().x : window.getSize().y);
    setBoardSize(board, {minDimension, minDimension});
    centerBoard(board, window);
    mainMenu.blocked = true;
    mainMenu.visible = false;
}

void resumeGame() {
    if (board.game.gameOver) {
        board = setUpBoard(true);
    }
    int minDimension = 0.9 * ((window.getSize().x < window.getSize().y) ? window.getSize().x : window.getSize().y);
    setBoardSize(board, {minDimension, minDimension});
    centerBoard(board, window);
    mainMenu.blocked = true;
    mainMenu.visible = false;
}

void setupMainMenu() {
    Button play;
    float scale = 0.3 * window.getSize().x / mainMenuButtonSize.x;
    mainMenu.blocked = false;
    mainMenu.visible = true;
    play.callback = startNewGame;
    play.normalTexture = mainMenuButton;
    play.sprite.setOrigin(mainMenuButtonSize/2.f);
    play.sprite.setPosition(sf::Vector2f(window.getSize())/2.f + sf::Vector2f(0, 0.5 * scale * mainMenuButtonSize.y));
    play.sprite.setScale(scale, scale);
    play.text.setString("Play");
    play.text.setFont(font);
    play.text.setCharacterSize(0.3 * scale * mainMenuButtonSize.y);
    play.text.setOrigin(play.text.getLocalBounds().getSize() / 2.f);
    play.text.setPosition(play.sprite.getPosition());
    mainMenu.buttons.push_back(play);
    Button resume;
    resume.callback = resumeGame;
    resume.normalTexture = mainMenuButton;
    resume.sprite.setOrigin(mainMenuButtonSize/2.f);
    resume.sprite.setPosition(sf::Vector2f(window.getSize())/2.f - sf::Vector2f(0, 0.5 * scale * mainMenuButtonSize.y));
    resume.sprite.setScale(scale, scale);
    resume.text.setString("Resume");
    resume.text.setFont(font);
    resume.text.setCharacterSize(0.3 * scale * mainMenuButtonSize.y);
    resume.text.setOrigin(resume.text.getLocalBounds().getSize() / 2.f);
    resume.text.setPosition(resume.sprite.getPosition());
    mainMenu.buttons.push_back(resume);
}

int main()
{

    mainMenuButton.loadFromFile("./assets/mainMenuButton.png");
    mainMenuButtonSize = sf::Vector2f(mainMenuButton.getSize());
    font.loadFromFile("./assets/go3v2.ttf");

    sf::VideoMode windowMode = sf::VideoMode::getDesktopMode();
    window.create(windowMode, "12 Beads Game", sf::Style::None);
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
    backSprite.setOrigin(back.getSize().x / 2.f, back.getSize().y / 2.f);
    backSprite.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
    cover(window, backSprite);

    sf::Sprite leftSprite;
    leftSprite.setTexture(left);
    fillHeight(window, leftSprite);

    sf::Sprite rightSprite;
    rightSprite.setTexture(right);
    rightSprite.setOrigin(right.getSize().x, 0);
    rightSprite.setPosition(window.getSize().x, 0);
    fillHeight(window, rightSprite);

    setupMainMenu();

    int possibleMoves[3][3];

    sf::Cursor cursor;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.control && event.key.code == sf::Keyboard::Q)
                {
                    window.close();
                }
            }
            else if (event.type == sf::Event::MouseMoved)
            {

                cursor.loadFromSystem(sf::Cursor::Arrow);
                if (checkHover(board, event.mouseMove) || checkHover(mainMenu, event.mouseMove) || checkHover(pauseMenu, event.mouseMove))
                {
                    cursor.loadFromSystem(sf::Cursor::Hand);
                }
                window.setMouseCursor(cursor);
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                checkClick(board, event.mouseButton);
                checkClick(mainMenu, event.mouseButton);
                checkClick(pauseMenu, event.mouseButton);
            }
        }

        window.clear();

        window.draw(backSprite);
        window.draw(leftSprite);
        window.draw(rightSprite);
        drawMenu(window, mainMenu);
        drawBoard(window, board);
        drawMenu(window, pauseMenu);

        window.display();
    }

    saveGameState(board.game);

    return 0;
}
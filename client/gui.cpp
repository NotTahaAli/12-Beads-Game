#include "gameHandler.h"
#include "menuHandler.h"

#include <iostream>
using namespace std;

Menu mainMenu, popupMenu;
Board board;

sf::Texture mainMenuButton, mainMenuButtonDisabled, mainMenuButtonHover, popup, backdrop, menuButtonImg;
sf::Vector2f mainMenuButtonSize, popupSize, menuButtonImgSize;
sf::Font font;
sf::Color normalColor(255, 255, 255, 255), disabledColor(0, 0, 0, 128), hoverColor(255, 255, 255, 255);

sf::RenderWindow window;

void setIcon(string fileLocation, sf::RenderWindow &window)
{
    sf::Image icon;
    icon.loadFromFile(fileLocation);
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
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

void showPopup(string message) {
    board.visible = true;
    board.blocked = true;
    mainMenu.visible = false;
    mainMenu.blocked = true;
    popupMenu.visible = true;
    popupMenu.blocked = false;
    popupMenu.buttons[0].text.setString(message);
    popupMenu.buttons[0].text.setOrigin(popupMenu.buttons[0].text.getLocalBounds().getSize() / 2.f);
}

void showMenu()
{
    board.visible = false;
    board.blocked = true;
    mainMenu.visible = true;
    mainMenu.blocked = false;
    popupMenu.visible = false;
    popupMenu.blocked = true;
    mainMenu.buttons[1].state = 1;
    saveGameState(board.game);
}

void startNewGame()
{
    board = setUpBoard(false);
    int minDimension = 0.9 * ((window.getSize().x < window.getSize().y) ? window.getSize().x : window.getSize().y);
    setBoardSize(board, {minDimension, minDimension});
    centerBoard(board, window);
    mainMenu.blocked = true;
    mainMenu.visible = false;
}

void resumeGame()
{
    if (board.game.gameOver)
    {
        board = setUpBoard(true);
    }
    int minDimension = 0.9 * ((window.getSize().x < window.getSize().y) ? window.getSize().x : window.getSize().y);
    setBoardSize(board, {minDimension, minDimension});
    centerBoard(board, window);
    board.visible = true;
    board.blocked = false;
    mainMenu.blocked = true;
    mainMenu.visible = false;
}

void setupMainMenu()
{
    Button play;
    float scale = 0.3 * window.getSize().x / mainMenuButtonSize.x;
    mainMenu.blocked = false;
    mainMenu.visible = true;
    play.normalTexture = mainMenuButton;
    play.disabledTexture = mainMenuButtonDisabled;
    play.hoverTexture = mainMenuButtonHover;
    play.normalColor = normalColor;
    play.disabledColor = disabledColor;
    play.hoverColor = hoverColor;
    play.sprite.setOrigin(mainMenuButtonSize / 2.f);
    play.sprite.setPosition(sf::Vector2f(window.getSize()) / 2.f + sf::Vector2f(0, 0.5 * scale * mainMenuButtonSize.y));
    play.sprite.setScale(scale, scale);
    play.text.setString("Play");
    play.text.setFont(font);
    play.text.setCharacterSize(0.3 * scale * mainMenuButtonSize.y);
    play.text.setOrigin(play.text.getLocalBounds().getSize() / 2.f);
    play.text.setPosition(play.sprite.getPosition());
    mainMenu.buttons.push_back(play);
    mainMenu.buttons[0].callback = startNewGame;
    Button resume;
    resume.state = 0;
    resume.normalTexture = mainMenuButton;
    resume.disabledTexture = mainMenuButtonDisabled;
    resume.hoverTexture = mainMenuButtonHover;
    resume.normalColor = normalColor;
    resume.disabledColor = disabledColor;
    resume.hoverColor = hoverColor;
    resume.sprite.setOrigin(mainMenuButtonSize / 2.f);
    resume.sprite.setPosition(sf::Vector2f(window.getSize()) / 2.f - sf::Vector2f(0, 0.5 * scale * mainMenuButtonSize.y));
    resume.sprite.setScale(scale, scale);
    resume.text.setString("Resume");
    resume.text.setFont(font);
    resume.text.setCharacterSize(0.3 * scale * mainMenuButtonSize.y);
    resume.text.setOrigin(resume.text.getLocalBounds().getSize() / 2.f);
    resume.text.setPosition(resume.sprite.getPosition());
    mainMenu.buttons.push_back(resume);
    mainMenu.buttons[1].callback = resumeGame;
}

void setupPopupMenu()
{
    float scale = 0.5 * window.getSize().x / popupSize.x;
    sf::Sprite backdropSprite;
    backdropSprite.setTexture(backdrop);
    cover(window, backdropSprite);
    popupMenu.backgroundElements.push_back(backdropSprite);
    sf::Sprite popupSprite;
    popupSprite.setTexture(popup);
    popupSprite.setOrigin(popupSize / 2.f);
    popupSprite.setPosition(sf::Vector2f(window.getSize()) / 2.f);
    popupSprite.setScale(scale, scale);
    popupMenu.backgroundElements.push_back(popupSprite);
    Button menuButton;
    scale = 0.1 * window.getSize().x / menuButtonImgSize.x;
    menuButton.normalTexture = menuButtonImg;
    menuButton.normalColor = normalColor;
    menuButton.disabledColor = normalColor;
    menuButton.hoverColor = normalColor;
    menuButton.sprite.setOrigin(popupSize / 2.f);
    menuButton.sprite.setPosition(sf::Vector2f(window.getSize()) / 2.f + sf::Vector2f(0, 0.5 * scale * menuButtonImgSize.y));
    menuButton.sprite.setScale(scale, scale);
    menuButton.text.setFont(font);
    menuButton.text.setCharacterSize(1.25 * scale * mainMenuButtonSize.y);
    menuButton.text.setPosition(menuButton.sprite.getPosition() - sf::Vector2f(0, scale * menuButtonImgSize.y));
    menuButton.callback = showMenu;
    popupMenu.buttons.push_back(menuButton);
}

int main()
{

    font.loadFromFile("./assets/go3v2.ttf");

    mainMenuButton.loadFromFile("./assets/mainMenuButton.png");
    mainMenuButtonDisabled.loadFromFile("./assets/mainMenuButtonDisabled.png");
    mainMenuButtonHover.loadFromFile("./assets/mainMenuButtonHover.png");
    mainMenuButtonSize = sf::Vector2f(mainMenuButton.getSize());
    menuButtonImg.loadFromFile("./assets/menuButton.png");
    menuButtonImgSize = sf::Vector2f(menuButtonImg.getSize());
    popup.loadFromFile("./assets/popup.png");
    popupSize = sf::Vector2f(popup.getSize());
    backdrop.loadFromFile("./assets/backdrop.png");

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
    setupPopupMenu();
    mainMenu.buttons[1].state = (isPreviousAvailable() ? 0 : 1);

    int possibleMoves[3][3];

    sf::Cursor cursor;

    while (window.isOpen())
    {
        if (!board.game.gameOver) {
            mainMenu.buttons[1].state = 0;
        }
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
                if (event.key.code == sf::Keyboard::Escape)
                {
                    showMenu();
                }
            }
            else if (event.type == sf::Event::MouseMoved)
            {

                cursor.loadFromSystem(sf::Cursor::Arrow);
                if (checkHover(board, event.mouseMove) || checkHover(mainMenu, event.mouseMove) || checkHover(popupMenu, event.mouseMove))
                {
                    cursor.loadFromSystem(sf::Cursor::Hand);
                }
                window.setMouseCursor(cursor);
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                checkClick(board, event.mouseButton, showPopup);
                checkClick(mainMenu, event.mouseButton);
                checkClick(popupMenu, event.mouseButton);
            }
        }

        window.clear();

        window.draw(backSprite);
        window.draw(leftSprite);
        window.draw(rightSprite);
        drawMenu(window, mainMenu);
        drawBoard(window, board);
        drawMenu(window, popupMenu);

        window.display();
    }

    if (!board.game.gameOver)
        saveGameState(board.game);

    return 0;
}
#include "spriteHandler.h"
#include "../common/game.h"

// Define Texture Variables
sf::Texture redBeadTexture;
sf::Texture blueBeadTexture;
sf::Texture boardTexture;

// Define Texture Sizes
sf::Vector2u redBeadTextureSize;
sf::Vector2u blueBeadTextureSize;
sf::Vector2u boardTextureSize;

#include <iostream>
using namespace std;

int floor (double num) {
    return ((num < 0) ? num-1 : num);
}

sf::Vector2f getGridBoxSize(Board &board)
{
    sf::Vector2f boardScale = board.sprite.getScale();
    return sf::Vector2f({boardScale.x * boardTextureSize.x, boardScale.y * boardTextureSize.y}) / 10.f;
};

// Get textures from image files
void initTextures()
{
    redBeadTexture.loadFromFile("assets/redBead.png");
    redBeadTextureSize = redBeadTexture.getSize();
    blueBeadTexture.loadFromFile("assets/blueBead.png");
    blueBeadTextureSize = blueBeadTexture.getSize();
    boardTexture.loadFromFile("assets/board.png");
    boardTextureSize = boardTexture.getSize();
}

// If `isRed == true`, get Red Bead Sprite
// If `isRed == false`, get Blue Bead Sprite
sf::Sprite getBeadSprite(bool isRed)
{
    sf::Sprite sprite;
    sprite.setTexture(((isRed) ? redBeadTexture : blueBeadTexture), true);
    return sprite;
}

sf::Sprite getBoardSprite()
{
    sf::Sprite sprite;
    sprite.setTexture(boardTexture);
    return sprite;
}

Bead createBead(bool isRed)
{
    return Bead({getBeadSprite(isRed),
                 {0, 0}});
}

void moveBead(Bead &bead, sf::Vector2i newGridPos, unsigned int frames) {
    bead.gridTarget = sf::Vector2f(newGridPos);
    bead.speed = (bead.gridTarget - bead.gridPos) / (float) frames;
}


Board setUpBoard()
{
    if (boardTextureSize.x == 0)
    {
        initTextures();
    }
    initGame();
    Board board = { getBoardSprite() };
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            int value = getValueAtPosition({x, y});
            if (value == 0)
                continue;
            board.beads[x][y] = createBead((value == 1));
            board.beads[x][y].gridPos = {x, y};
            board.beads[x][y].gridTarget = {x, y};
        }
    }
    return board;
}

void updateBeadPosition(Board &board, Bead &bead)
{

    if (bead.gridPos != bead.gridTarget)
    {
        bead.gridPos += bead.speed;
        if (bead.gridPos.x * bead.speed.x > bead.gridTarget.x * bead.speed.x || bead.gridPos.y * bead.speed.y > bead.gridTarget.y * bead.speed.y)
        {
            bead.speed = {0, 0};
            bead.gridPos = sf::Vector2f(sf::Vector2i(bead.gridPos));
        }
    }

    sf::Vector2f gridBoxSize = getGridBoxSize(board);
    sf::Vector2f gap = gridBoxSize*5.f / 6.f + gridBoxSize;
    sf::Vector2f boardPos = board.sprite.getPosition();
    boardPos += gridBoxSize*5.f / 6.f;
    sf::Vector2i textureSize = bead.sprite.getTextureRect().getSize();
    bead.sprite.setPosition({boardPos.x + gap.x * bead.gridPos.x, boardPos.y + gap.y * bead.gridPos.y});
    bead.sprite.setScale({gridBoxSize.x / textureSize.x, gridBoxSize.y / textureSize.y});
}

void drawBoard(sf::RenderWindow &window, Board &board)
{
    window.draw(board.sprite);
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            if (board.beads[x][y].sprite.getTexture() == nullptr)
                continue;
            updateBeadPosition(board, board.beads[x][y]);
            window.draw(board.beads[x][y].sprite);
        }
    }
}

void setBoardSize(Board &board, sf::Vector2i SizeInPixels)
{
    board.sprite.setScale({(float)SizeInPixels.x / boardTextureSize.x, (float)SizeInPixels.y / boardTextureSize.y});
}

void centerBoard(Board &board, sf::RenderWindow &window)
{
    sf::Vector2f boardSize = getGridBoxSize(board) * 10.0f;
    board.sprite.setPosition({(window.getSize().x - boardSize.x) / 2.f, (window.getSize().y - boardSize.y) / 2.f});
}

void checkClick(Board &board, sf::Event event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == 0) {
            sf::Vector2f gridBoxSize = getGridBoxSize(board);
            sf::Vector2f gap = gridBoxSize*5.f / 6.f + gridBoxSize;
            sf::Vector2f boardPos = board.sprite.getPosition();
            sf::Vector2i gridPos = {floor((event.mouseButton.x - boardPos.x)/gap.x), floor((event.mouseButton.y - boardPos.y)/gap.y)};
            boardPos += gridBoxSize*5.f / 6.f;
            if (gridPos.x >= 0 && gridPos.y >= 0 && gridPos.x < 5 && gridPos.y < 5) {
                sf::Vector2f Center = {boardPos.x + gap.x * gridPos.x, boardPos.y + gap.y * gridPos.y};
                Center += gridBoxSize / 2.f;
                // Checking If Click Inside Circle
                if (((event.mouseButton.x - Center.x)*(event.mouseButton.x - Center.x))/((gridBoxSize.x / 2.f)*(gridBoxSize.x / 2.f))
                + ((event.mouseButton.y - Center.y)*(event.mouseButton.y - Center.y))/((gridBoxSize.y / 2.f)*(gridBoxSize.y / 2.f)) <= 1) {
                    // Check if Highlight. If Highlight then try the move, else check if bead and show highlights.
                    cout << gridPos.x << " " << gridPos.y << endl;
                }
            }
        }
    }
}
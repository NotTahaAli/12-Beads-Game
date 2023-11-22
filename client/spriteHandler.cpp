#include "spriteHandler.h"
#include <cmath>

// Define Texture Variables
sf::Texture redBeadTexture;
sf::Texture blueBeadTexture;
sf::Texture boardTexture;

// Define Texture Sizes
sf::Vector2u redBeadTextureSize;
sf::Vector2u blueBeadTextureSize;
sf::Vector2u boardTextureSize;

const float boardDivisions = 10;

#include <iostream>
using namespace std;

sf::Vector2f getGridBoxSize(Board &board)
{
    sf::Vector2f boardScale = board.sprite.getScale();
    return sf::Vector2f({boardScale.x * boardTextureSize.x, boardScale.y * boardTextureSize.y}) / boardDivisions;
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

void moveBead(Bead &bead, sf::Vector2i newGridPos, unsigned int frames)
{
    bead.gridTarget = sf::Vector2f(newGridPos);
    bead.speed = (bead.gridTarget - bead.gridPos) / (float)frames;
}

void removeBead(Bead &bead, unsigned int frames) {
    bead.gridTarget = sf::Vector2f(bead.gridPos.x, -2);
    bead.speed = (bead.gridTarget - bead.gridPos) / (float)frames;
}

Board setUpBoard()
{
    if (boardTextureSize.x == 0)
    {
        initTextures();
    }
    Board board = {getBoardSprite()};
    board.game = initGame();
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            int value = getValueAtPosition(board.game, {x, y});
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
    sf::Vector2f gap = gridBoxSize * (boardDivisions - 5) / 6.f + gridBoxSize;
    sf::Vector2f boardPos = board.sprite.getPosition();
    boardPos += gridBoxSize * (boardDivisions - 5) / 6.f;
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
    sf::Vector2f boardSize = getGridBoxSize(board) * boardDivisions;
    board.sprite.setPosition({(window.getSize().x - boardSize.x) / 2.f, (window.getSize().y - boardSize.y) / 2.f});
}

sf::Vector2i getCircularGridPos(Board &board, sf::Vector2i pos)
{
    sf::Vector2f gridBoxSize = getGridBoxSize(board);
    sf::Vector2f gap = gridBoxSize * (boardDivisions - 5) / 6.f + gridBoxSize;
    sf::Vector2f boardPos = board.sprite.getPosition();
    sf::Vector2i gridPos = {floor((pos.x - boardPos.x) / gap.x), floor((pos.y - boardPos.y) / gap.y)};
    boardPos += gridBoxSize * (boardDivisions - 5) / 6.f;
    if (gridPos.x >= 0 && gridPos.y >= 0 && gridPos.x < 5 && gridPos.y < 5)
    {
        sf::Vector2f Center = {boardPos.x + gap.x * gridPos.x, boardPos.y + gap.y * gridPos.y};
        Center += gridBoxSize / 2.f;
        // Checking If Position Inside Ellipse
        if (pow(pos.x - Center.x, 2) / pow(gridBoxSize.x / 2, 2) +
                pow(pos.y - Center.y, 2) / pow(gridBoxSize.y / 2, 2) <=
            1)
        {
            return {gridPos.x, gridPos.y};
        }
    }
    return {-1, -1};
}

void checkClick(Board &board, sf::Event event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == 0)
        {
            sf::Vector2i gridPos = getCircularGridPos(board, {event.mouseButton.x, event.mouseButton.y});
            if (gridPos.x >= 0) {

            }
        }
    }
}
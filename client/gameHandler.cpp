#include "socketHandler.h"
#include <cmath>

int currentPlayer = 0;

// Define Texture Variables
sf::Texture redBeadTexture;
sf::Texture blueBeadTexture;
sf::Texture boardTexture;
sf::Texture highlightTexture;

// Define Texture Sizes
sf::Vector2u redBeadTextureSize;
sf::Vector2u blueBeadTextureSize;
sf::Vector2u boardTextureSize;
sf::Vector2u highlightTextureSize;

sf::SoundBuffer captureSoundBuffer, moveSoundBuffer, playerHighlightSoundBuffer, defeatSoundBuffer, victorySoundBuffer;
sf::Sound captureSound, moveSound, playerHighlightSound, defeatSound, victorySound;

const float boardDivisions = 10;
const float outlineThickness = 5;
sf::Color highlightColor(255,255,255,128);

#include <iostream>
using namespace std;

void updateHighlightsPosition(Board &board);

sf::Vector2f getGridBoxSize(Board &board)
{
    sf::Vector2f boardScale = board.sprite.getScale();
    return sf::Vector2f({boardScale.x * boardTextureSize.x, boardScale.y * boardTextureSize.y}) / boardDivisions;
};

// Get textures from image files
void initTextures()
{
    captureSoundBuffer.loadFromFile("assets/Capture.ogg");
    captureSound.setBuffer(captureSoundBuffer);
    moveSoundBuffer.loadFromFile("assets/Move.ogg");
    moveSound.setBuffer(moveSoundBuffer);
    playerHighlightSoundBuffer.loadFromFile("assets/Highlight.ogg");
    playerHighlightSound.setBuffer(playerHighlightSoundBuffer);
    defeatSoundBuffer.loadFromFile("assets/defeat.ogg");
    defeatSound.setBuffer(defeatSoundBuffer);
    victorySoundBuffer.loadFromFile("assets/victory.ogg");
    victorySound.setBuffer(victorySoundBuffer);
    redBeadTexture.loadFromFile("assets/redBead.png");
    redBeadTextureSize = redBeadTexture.getSize();
    blueBeadTexture.loadFromFile("assets/blueBead.png");
    blueBeadTextureSize = blueBeadTexture.getSize();
    boardTexture.loadFromFile("assets/board.png");
    boardTextureSize = boardTexture.getSize();
    highlightTexture.loadFromFile("assets/highlight.png");
    highlightTextureSize = highlightTexture.getSize();
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

void moveBead(Board &board, Bead &bead, sf::Vector2i newGridPos, unsigned int frames)
{
    if (backgroundMusic.getStatus() == sf::Music::Status::Playing) {
        moveSound.play();
    }
    bead.gridTarget = sf::Vector2f(newGridPos);
    bead.speed = (bead.gridTarget - bead.gridPos) / (float)frames;
    board.beads[newGridPos.x][newGridPos.y] = bead;
    bead = Bead();
}

void removeBead(Bead &bead, unsigned int frames)
{
    if (backgroundMusic.getStatus() == sf::Music::Status::Playing) {
        captureSound.play();
    }
    bead.gridTarget = sf::Vector2f(bead.gridPos.x, -2);
    bead.speed = (bead.gridTarget - bead.gridPos) / (float)frames;
}

void showPlayerHighlight(Board &board, sf::Vector2i gridPos) {
    if (backgroundMusic.getStatus() == sf::Music::Status::Playing) {
        playerHighlightSound.play();
    }
    board.playerBeadHighlight.setOutlineThickness(outlineThickness);
    sf::Vector2f gridBoxSize = getGridBoxSize(board);
    sf::Vector2f gap = gridBoxSize * (boardDivisions - 5) / 6.f + gridBoxSize;
    sf::Vector2f boardPos = board.sprite.getPosition();
    boardPos += gridBoxSize * (boardDivisions - 5) / 6.f;
    board.playerBeadHighlight.setPosition({boardPos.x + gap.x * gridPos.x, boardPos.y + gap.y * gridPos.y});
    board.playerBeadHighlight.setRadius(gridBoxSize.x/2);
    board.playerBeadHighlight.setScale(1, gridBoxSize.x / gridBoxSize.y);
}

Board setUpBoard(bool loadPrevious)
{
    if (boardTextureSize.x == 0)
    {
        initTextures();
    }
    Board board = {getBoardSprite()};
    board.game = initGame(loadPrevious);
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
    sf::Vector2f gridSize = getGridBoxSize(board);
    board.playerBeadHighlight.setRadius(gridSize.x/2);
    board.playerBeadHighlight.setScale(1, gridSize.x / gridSize.y);
    board.playerBeadHighlight.setFillColor(sf::Color::Transparent);
    board.playerBeadHighlight.setOutlineColor(highlightColor);
    board.playerBeadHighlight.setOutlineThickness(0);
    updateHighlightsPosition(board);
    board.visible = true;
    board.blocked = false;
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
            bead.gridPos = sf::Vector2f(roundf(bead.gridPos.x), roundf(bead.gridPos.y));
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
    if (!board.visible)
        return;
    window.draw(board.sprite);
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            if (board.beads[x][y].sprite.getTexture() != nullptr)
            {
                updateBeadPosition(board, board.beads[x][y]);
                window.draw(board.beads[x][y].sprite);
            }
            if (board.highlights[x][y].sprite.getScale().x != 0)
                window.draw(board.highlights[x][y].sprite);
        }
    }
    window.draw(board.playerBeadHighlight);
}

void setBoardSize(Board &board, sf::Vector2i SizeInPixels)
{
    board.sprite.setScale({(float)SizeInPixels.x / boardTextureSize.x, (float)SizeInPixels.y / boardTextureSize.y});
    updateHighlightsPosition(board);
}

void centerBoard(Board &board, sf::RenderWindow &window)
{
    sf::Vector2f boardSize = getGridBoxSize(board) * boardDivisions;
    board.sprite.setPosition({(window.getSize().x - boardSize.x) / 2.f, (window.getSize().y - boardSize.y) / 2.f});
    updateHighlightsPosition(board);
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

bool isHighlightValid(Board &board, sf::Vector2i gridPos)
{
    sf::Vector2i move = board.highlights[gridPos.x][gridPos.y].move;
    return (move.x >= 0 && move.y >= 0 && !(move.x == 1 && move.y == 1));
}

bool isBeadClickValid(Board &board, sf::Vector2i gridPos)
{
    int turn = getTurn(board.game);
    return (turn != 0 && (board.game.lastTurn.x == -1 && board.game.lastTurn.y == -1 || (board.game.lastTurn.x == gridPos.x && board.game.lastTurn.y == gridPos.y)) && getValueAtPosition(board.game, {gridPos.x, gridPos.y}) == ((board.isOnline) ? currentPlayer : turn) && (!board.isOnline || currentPlayer != 0));
}

bool checkHover(Board &board, sf::Event::MouseMoveEvent mouseMove)
{
    bool hovering = false;
    if (!board.blocked)
    {
        sf::Vector2i gridPos = getCircularGridPos(board, {mouseMove.x, mouseMove.y});
        if (gridPos.x >= 0 && (isHighlightValid(board, gridPos) || isBeadClickValid(board, gridPos)))
            hovering = true;
    };
    return hovering;
}

void resetHighlights(Board &board)
{
    board.playerBeadHighlight.setOutlineThickness(0);
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            board.highlights[i][j].move = {-1, -1};
            board.highlights[i][j].sprite.setScale(0, 0);
            board.highlights[i][j].base = {-1, -1};
        }
    }
}

void updateHighlightsPosition(Board &board)
{
    sf::Vector2f gridBoxSize = getGridBoxSize(board);
    sf::Vector2f gap = gridBoxSize * (boardDivisions - 5) / 6.f + gridBoxSize;
    sf::Vector2f boardPos = board.sprite.getPosition();
    boardPos += gridBoxSize * (boardDivisions - 5) / 6.f;
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            board.highlights[x][y].sprite.setPosition({boardPos.x + gap.x * x, boardPos.y + gap.y * y});
            board.highlights[x][y].sprite.setTexture(highlightTexture);
            board.highlights[x][y].sprite.setScale(0, 0);
        }
    }
}

void showHighlight(Board &board, sf::Vector2i gridPos, sf::Vector2i move, int moveVal)
{
    sf::Vector2f gridBoxSize = getGridBoxSize(board);
    showPlayerHighlight(board, gridPos);
    Highlight &highlight = board.highlights[gridPos.x + moveVal * (move.x - 1)][gridPos.y + moveVal * (move.y - 1)];
    highlight.move.x = move.x;
    highlight.move.y = move.y;
    highlight.base = gridPos;
    highlight.sprite.setScale({gridBoxSize.x / highlightTextureSize.x, gridBoxSize.y / highlightTextureSize.y});
}

void renderHighlights(Board &board, sf::Vector2i gridPos)
{
    if (!board.isOnline)
    {
        int moves[3][3];
        getMoves(board.game, {gridPos.x, gridPos.y}, moves);
        resetHighlights(board);
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (moves[i][j] != 0)
                {
                    showHighlight(board, gridPos, {i, j}, moves[i][j]);
                }
            }
        }
    }
    else
    {
        sendGetMoves(gridPos);
    }
}

void attemptTurnPlay(Board &board, sf::Vector2i gridPos)
{
    Highlight highlight = board.highlights[gridPos.x][gridPos.y];
    if (!board.isOnline)
    {
        turnData tdata = playTurn(board.game, {highlight.base.x, highlight.base.y}, {highlight.move.x, highlight.move.y});
        resetHighlights(board);
        if (tdata.status == 0)
        {
            // Move Error
            return;
        }
        moveBead(board, board.beads[tdata.move.from.x][tdata.move.from.y], {tdata.move.to.x, tdata.move.to.y});
        // board.beads[tdata.move.to.x][tdata.move.to.y] = board.beads[tdata.move.from.x][tdata.move.from.y];
        // board.beads[tdata.move.from.x][tdata.move.from.y] = Bead();
        if (tdata.remove.from.x >= 0)
        {
            removeBead(board.beads[tdata.remove.from.x][tdata.remove.from.y]);
        }
        if (tdata.status == 3)
        {
            if (backgroundMusic.getStatus() == sf::Music::Status::Playing) {
                victorySound.play();
            }
            showPopup((checkVictory(board.game) == 1) ? "Black Won" : "Red Won");
            board.blocked = true;
        }
        else if (tdata.status == 1)
        {
            renderHighlights(board, {tdata.move.to.x, tdata.move.to.y});
        }
    }
    else
    {
        sendPlayMove({highlight.base.x, highlight.base.y}, {highlight.move.x, highlight.move.y});
    }
}

void checkClick(Board &board, sf::Event::MouseButtonEvent mouseButton)
{
    if (mouseButton.button == 0)
    {
        if (!board.blocked)
        {
            sf::Vector2i gridPos = getCircularGridPos(board, {mouseButton.x, mouseButton.y});
            if (gridPos.x >= 0)
            {
                if (isHighlightValid(board, gridPos))
                {
                    attemptTurnPlay(board, gridPos);
                }
                else if (isBeadClickValid(board, gridPos))
                    renderHighlights(board, gridPos);
            }
        }
    }
}
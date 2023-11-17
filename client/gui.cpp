#include <SFML/Graphics.hpp>

#include <iostream>
using namespace std;

float vw(float percentage, sf::RenderWindow &window) {
    return percentage/100 * window.getSize().x;
}

float vh(float percentage, sf::RenderWindow &window) {
    return percentage/100 * window.getSize().y;
}

void setIcon(string fileLocation, sf::RenderWindow &window) {
    sf::Image icon;
    icon.loadFromFile(fileLocation);
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
}

sf::Sprite getImageSprite(string fileLocation, sf::Vector2f pos = {0,0}) {
    static sf::Texture texture;
    texture.loadFromFile(fileLocation);
    sf::Sprite sprite;
    sprite.setTexture(texture, true);
    sprite.setPosition(pos);
    return sprite;
}

void centerAlignSprite(sf::Sprite &sprite) {
    sprite.setPosition(sprite.getPosition() - sf::Vector2f(sprite.getTextureRect().getSize())/2.f);
}

sf::Sprite centerAlignSpriteInline(sf::Sprite sprite) {
    centerAlignSprite(sprite);
    return sprite;
}

void setSpriteSize(sf::Sprite &sprite, sf::Vector2i size) {
    sf::Vector2i textureSize = sprite.getTextureRect().getSize();
    sprite.setScale(sf::Vector2f((float)size.x/textureSize.x, (float)size.y/textureSize.y));
}

sf::Sprite setSpriteSizeInline(sf::Sprite sprite, sf::Vector2i size) {
    setSpriteSize(sprite,size);
    return sprite;
}

int main()
{
    sf::VideoMode windowMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(windowMode, "12 Beads Game", sf::Style::None);
    setIcon("./assets/icon.jpg",window);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        
        window.draw(getImageSprite("./assets/icon.jpg"));
        window.draw(centerAlignSpriteInline(getImageSprite("./assets/icon2.png",sf::Vector2f(vw(50, window),vh(50, window)))));

        window.display();
    }

    return 0;
}
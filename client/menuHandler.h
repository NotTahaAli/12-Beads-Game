#include <SFML/Graphics.hpp>
#include <vector>

typedef void (*callback)();

struct Button {
    sf::Sprite sprite;
    sf::Texture normalTexture, disabledTexture, hoverTexture;
    sf::Text text;
    callback callback;
    // 0 - Normal
    // 1 - Disabled
    // 2 - Hovering
    int state = 0;
};

struct Menu
{
    std::vector<sf::Sprite> backgroundElements;
    std::vector<Button> buttons;
    bool visible = false;
    bool blocked = true;
};

void drawMenu(sf::RenderWindow &window, Menu &menu);

void checkClick(Menu &menu, sf::Event::MouseButtonEvent mouseButton);
bool checkHover(Menu &menu, sf::Event::MouseMoveEvent mouseMove);
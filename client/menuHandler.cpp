#include "menuHandler.h"

void drawMenu(sf::RenderWindow &window, Menu &menu)
{
    if (!menu.visible)
        return;
    for (int i = 0; i < menu.backgroundElements.size(); i++)
    {
        window.draw(menu.backgroundElements[i]);
    }
    for (int i = 0; i < menu.buttons.size(); i++)
    {
        if (menu.buttons[i].state == 0)
        {
            menu.buttons[i].sprite.setTexture(menu.buttons[i].normalTexture);
            menu.buttons[i].text.setFillColor(menu.buttons[i].normalColor);
        }
        else if (menu.buttons[i].state == 1)
        {
            menu.buttons[i].sprite.setTexture(menu.buttons[i].disabledTexture);
            menu.buttons[i].text.setFillColor(menu.buttons[i].disabledColor);
        }
        else
        {
            menu.buttons[i].sprite.setTexture(menu.buttons[i].hoverTexture);
            menu.buttons[i].text.setFillColor(menu.buttons[i].hoverColor);
        }
        window.draw(menu.buttons[i].sprite);
        window.draw(menu.buttons[i].text);
    }
}

void checkClick(Menu &menu, sf::Event::MouseButtonEvent mouseButton)
{
    if (mouseButton.button == 1)
        return;
    if (menu.blocked || !menu.visible)
        return;
    for (int i = 0; i < menu.buttons.size(); i++)
    {
        if (menu.buttons[i].sprite.getGlobalBounds().contains(mouseButton.x, mouseButton.y))
        {
            if (menu.buttons[i].state == 1)
                return;
            menu.buttons[i].callback();
            return;
        }
    }
};
bool checkHover(Menu &menu, sf::Event::MouseMoveEvent mouseMove)
{
    if (menu.blocked || !menu.visible)
        return false;
    bool hovering = false;
    for (int i = 0; i < menu.buttons.size(); i++)
    {
        if (menu.buttons[i].sprite.getGlobalBounds().contains(mouseMove.x, mouseMove.y))
        {
            if (menu.buttons[i].state == 1)
                continue;
            hovering = true;
            menu.buttons[i].state == 2;
            continue;
        }
        else if (menu.buttons[i].state != 1)
            menu.buttons[i].state = 0;
    }
    return hovering;
};
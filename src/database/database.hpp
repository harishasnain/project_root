#pragma once

#include <SFML/Graphics.hpp>

class Database {
public:
    Database(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void draw();

private:
    sf::RenderWindow& m_window;
    sf::RectangleShape m_background;
    sf::RectangleShape m_exitButton;
};
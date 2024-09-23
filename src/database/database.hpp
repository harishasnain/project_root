#pragma once

#include <SFML/Graphics.hpp>

class Database {
public:
    Database(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window);
    void resetShouldExit() { m_shouldExit = false; }
    bool shouldReturnToMain() const { return m_shouldExit; }

private:
    sf::RenderWindow& m_window;
    sf::RectangleShape m_background;
    sf::RectangleShape m_exitButton;
    bool m_shouldExit = false;
};
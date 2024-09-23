#pragma once

#include <SFML/Graphics.hpp>

class Camera {
public:
    Camera(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void draw();

private:
    sf::RenderWindow& m_window;
    sf::RectangleShape m_background;
    sf::RectangleShape m_exitButton;
};
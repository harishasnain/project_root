#include "camera.hpp"
#include <iostream>

Camera::Camera(sf::RenderWindow& window)
    : m_window(window)
{
    m_background = sf::RectangleShape(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
    m_background.setFillColor(sf::Color::Blue);

    m_exitButton = sf::RectangleShape(sf::Vector2f(50, 50));
    m_exitButton.setPosition(10, 10);
    m_exitButton.setFillColor(sf::Color::White);
    m_exitButton.setOutlineThickness(2);
    m_exitButton.setOutlineColor(sf::Color::Black);
}

void Camera::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

            if (m_exitButton.getGlobalBounds().contains(mousePos)) {
                // Exit camera app
                m_shouldExit = true;
            }
        }
    }
}

void Camera::draw(sf::RenderWindow& window) {
    m_window.draw(m_background);
    m_window.draw(m_exitButton);
}
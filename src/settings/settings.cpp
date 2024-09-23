#include "settings.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

Settings::Settings(sf::RenderWindow& window)
    : m_window(window),
      m_isMetricSystem(true),
      m_isLowPowerMode(false),
      m_password("")
{
    if (!m_font.loadFromFile("resources/fonts/Roboto-Regular.ttf")) {
       // Handle font loading error
       std::cerr << "Failed to load font" << std::endl;
	}

    m_exitButton = sf::RectangleShape(sf::Vector2f(50, 50));
    m_exitButton.setPosition(10, 10);
    m_exitButton.setFillColor(sf::Color::White);
    m_exitButton.setOutlineThickness(2);
    m_exitButton.setOutlineColor(sf::Color::Black);

    // Initialize text and buttons for each setting
    m_timeText.setFont(m_font);
    m_timeText.setCharacterSize(24);
    m_timeText.setFillColor(sf::Color::Black);
    m_timeText.setPosition(100, 100);
    m_timeText.setString("Adjust Time");

    m_timeButton = sf::RectangleShape(sf::Vector2f(200, 50));
    m_timeButton.setPosition(300, 100);
    m_timeButton.setFillColor(sf::Color::White);
    m_timeButton.setOutlineThickness(2);
    m_timeButton.setOutlineColor(sf::Color::Black);

    m_dateText.setFont(m_font);
    m_dateText.setCharacterSize(24);
    m_dateText.setFillColor(sf::Color::Black);
    m_dateText.setPosition(100, 200);
    m_dateText.setString("Adjust Date");

    m_dateButton = sf::RectangleShape(sf::Vector2f(200, 50));
    m_dateButton.setPosition(300, 200);
    m_dateButton.setFillColor(sf::Color::White);
    m_dateButton.setOutlineThickness(2);
    m_dateButton.setOutlineColor(sf::Color::Black);

    m_metricSystemText.setFont(m_font);
    m_metricSystemText.setCharacterSize(24);
    m_metricSystemText.setFillColor(sf::Color::Black);
    m_metricSystemText.setPosition(100, 300);
    m_metricSystemText.setString("Metric System: ON");

    m_metricSystemToggle = sf::RectangleShape(sf::Vector2f(100, 50));
    m_metricSystemToggle.setPosition(400, 300);
    m_metricSystemToggle.setFillColor(sf::Color::Green);

    m_lowPowerModeText.setFont(m_font);
    m_lowPowerModeText.setCharacterSize(24);
    m_lowPowerModeText.setFillColor(sf::Color::Black);
    m_lowPowerModeText.setPosition(100, 400);
    m_lowPowerModeText.setString("Low Power Mode: OFF");

    m_lowPowerModeToggle = sf::RectangleShape(sf::Vector2f(100, 50));
    m_lowPowerModeToggle.setPosition(400, 400);
    m_lowPowerModeToggle.setFillColor(sf::Color::Red);

    m_passwordText.setFont(m_font);
    m_passwordText.setCharacterSize(24);
    m_passwordText.setFillColor(sf::Color::Black);
    m_passwordText.setPosition(100, 500);
    m_passwordText.setString("Change Password");

    m_passwordButton = sf::RectangleShape(sf::Vector2f(200, 50));
    m_passwordButton.setPosition(300, 500);
    m_passwordButton.setFillColor(sf::Color::White);
    m_passwordButton.setOutlineThickness(2);
    m_passwordButton.setOutlineColor(sf::Color::Black);
}

void Settings::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

            if (m_exitButton.getGlobalBounds().contains(mousePos)) {
                // Exit settings app
                std::cout << "Exiting Settings" << std::endl;
            } else if (m_timeButton.getGlobalBounds().contains(mousePos)) {
                adjustTime();
            } else if (m_dateButton.getGlobalBounds().contains(mousePos)) {
                adjustDate();
            } else if (m_metricSystemToggle.getGlobalBounds().contains(mousePos)) {
                toggleMetricSystem();
            } else if (m_lowPowerModeToggle.getGlobalBounds().contains(mousePos)) {
                toggleLowPowerMode();
            } else if (m_passwordButton.getGlobalBounds().contains(mousePos)) {
                changePassword();
            }
        }
    }
}

void Settings::draw() {
    m_window.draw(m_exitButton);
    m_window.draw(m_timeText);
    m_window.draw(m_timeButton);
    m_window.draw(m_dateText);
    m_window.draw(m_dateButton);
    m_window.draw(m_metricSystemText);
    m_window.draw(m_metricSystemToggle);
    m_window.draw(m_lowPowerModeText);
    m_window.draw(m_lowPowerModeToggle);
    m_window.draw(m_passwordText);
    m_window.draw(m_passwordButton);
}

void Settings::toggleMetricSystem() {
    m_isMetricSystem = !m_isMetricSystem;
    m_metricSystemText.setString("Metric System: " + std::string(m_isMetricSystem ? "ON" : "OFF"));
    m_metricSystemToggle.setFillColor(m_isMetricSystem ? sf::Color::Green : sf::Color::Red);
    if (onMetricSystemChanged) {
        onMetricSystemChanged(m_isMetricSystem);
    }
}

void Settings::toggleLowPowerMode() {
    m_isLowPowerMode = !m_isLowPowerMode;
    m_lowPowerModeText.setString("Low Power Mode: " + std::string(m_isLowPowerMode ? "ON" : "OFF"));
    m_lowPowerModeToggle.setFillColor(m_isLowPowerMode ? sf::Color::Green : sf::Color::Red);
    if (onLowPowerModeChanged) {
        onLowPowerModeChanged(m_isLowPowerMode);
    }
}

void Settings::changePassword() {
    // In a real application, you'd open a dialog for password input
    m_password = "newpassword";
    if (onPasswordChanged) {
        onPasswordChanged(m_password);
    }
}

void Settings::adjustTime() {
    // In a real application, you'd open a time picker dialog
    sf::Time newTime = sf::seconds(12 * 3600 + 30 * 60); // 12:30:00
    if (onTimeChanged) {
        onTimeChanged(newTime);
    }
}

void Settings::adjustDate() {
    // In a real application, you'd open a date picker dialog
    sf::Time newDate = sf::seconds(1234567890); // Example timestamp
    if (onDateChanged) {
        onDateChanged(newDate);
    }
}
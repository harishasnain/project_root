#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

class Settings {
public:
    Settings(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window);
    void resetShouldExit() { m_shouldExit = false; }

    // Callback functions for main window
    std::function<void(const sf::Time&)> onTimeChanged;
    std::function<void(const sf::Time&)> onDateChanged;
    std::function<void(bool)> onMetricSystemChanged;
    std::function<void(bool)> onLowPowerModeChanged;
    std::function<void(const std::string&)> onPasswordChanged;
    bool shouldReturnToMain() const { return m_shouldExit; }

private:
    sf::RenderWindow& m_window;
    sf::Font m_font;
    sf::RectangleShape m_exitButton;

    sf::Text m_timeText;
    sf::Text m_dateText;
    sf::Text m_metricSystemText;
    sf::Text m_lowPowerModeText;
    sf::Text m_passwordText;

    sf::RectangleShape m_timeButton;
    sf::RectangleShape m_dateButton;
    sf::RectangleShape m_metricSystemToggle;
    sf::RectangleShape m_lowPowerModeToggle;
    sf::RectangleShape m_passwordButton;

    bool m_isMetricSystem;
    bool m_isLowPowerMode;
    std::string m_password;

    void toggleMetricSystem();
    void toggleLowPowerMode();
    void changePassword();
    void adjustTime();
    void adjustDate();
    bool m_shouldExit = false;
};
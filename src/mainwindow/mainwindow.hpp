#pragma once

#include <SFML/Graphics.hpp>
#include "../map/map.hpp"
#include "../chatbot/chatbot.hpp"
#include "../database/database.hpp"
#include "../camera/camera.hpp"
#include "../settings/settings.hpp"
#include "../utils/weather.hpp"
#include <memory>
#include <string>

class MainWindow {
public:
    MainWindow(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window);

private:
    sf::RenderWindow& m_window;
    sf::Font m_font;
    sf::Text m_timeText;
    sf::Text m_weatherText;

    std::unique_ptr<Map> m_map;
    std::unique_ptr<Chatbot> m_chatbot;
    std::unique_ptr<Database> m_database;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Settings> m_settings;
    Weather m_weather;

    enum class ActiveApp {
        None,
        Map,
        Chatbot,
        Database,
        Camera,
        Settings
    };

    ActiveApp m_activeApp;

    std::vector<sf::RectangleShape> m_appButtons;
    std::vector<sf::Text> m_appButtonTexts;

    bool m_isPasswordProtected;
    std::string m_password;
    bool m_isPasswordEntered;
    sf::Text m_passwordPrompt;
    sf::Text m_passwordInput;

    void updateTimeAndWeather();
    void drawAppButtons();
    void initializeAppButtons();
    void handleAppButtonClick(sf::Vector2f mousePos);
    void switchToApp(ActiveApp app);

    void promptPassword();
    void handlePasswordInput(const sf::Event& event);
    bool checkPassword(const std::string& input);

    void onTimeChanged(const sf::Time& newTime);
    void onDateChanged(const sf::Time& newDate);
    void onMetricSystemChanged(bool isMetric);
    void onLowPowerModeChanged(bool isLowPower);
    void onPasswordChanged(const std::string& newPassword);
};
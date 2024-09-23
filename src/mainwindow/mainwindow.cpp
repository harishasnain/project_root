#include "mainwindow.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

MainWindow::MainWindow(sf::RenderWindow& window)
    : m_window(window),
      m_activeApp(ActiveApp::None),
      m_isPasswordProtected(false),
      m_isPasswordEntered(false)
{
    if (!m_font.loadFromFile("resources/fonts/Roboto-Regular.ttf")) {
        // Handle font loading error
		std::cerr << "Failed to load font" << std::endl;
    }

    m_timeText.setFont(m_font);
    m_timeText.setCharacterSize(24);
    m_timeText.setFillColor(sf::Color::Black);
    m_timeText.setPosition(800, 10);

    m_weatherText.setFont(m_font);
    m_weatherText.setCharacterSize(24);
    m_weatherText.setFillColor(sf::Color::Black);
    m_weatherText.setPosition(800, 50);

    m_map = std::make_unique<Map>(window);
    m_chatbot = std::make_unique<Chatbot>(window);
    m_database = std::make_unique<Database>(window);
    m_camera = std::make_unique<Camera>(window);
    m_settings = std::make_unique<Settings>(window);

    m_settings->onTimeChanged = [this](const sf::Time& newTime) { onTimeChanged(newTime); };
    m_settings->onDateChanged = [this](const sf::Time& newDate) { onDateChanged(newDate); };
    m_settings->onMetricSystemChanged = [this](bool isMetric) { onMetricSystemChanged(isMetric); };
    m_settings->onLowPowerModeChanged = [this](bool isLowPower) { onLowPowerModeChanged(isLowPower); };
    m_settings->onPasswordChanged = [this](const std::string& newPassword) { onPasswordChanged(newPassword); };

    initializeAppButtons();
    updateTimeAndWeather();

    m_passwordPrompt.setFont(m_font);
    m_passwordPrompt.setCharacterSize(24);
    m_passwordPrompt.setFillColor(sf::Color::Black);
    m_passwordPrompt.setPosition(400, 300);
    m_passwordPrompt.setString("Enter Password:");

    m_passwordInput.setFont(m_font);
    m_passwordInput.setCharacterSize(24);
    m_passwordInput.setFillColor(sf::Color::Black);
    m_passwordInput.setPosition(400, 350);
}

void MainWindow::handleEvent(const sf::Event& event) {
    if (m_isPasswordProtected && !m_isPasswordEntered) {
        handlePasswordInput(event);
        return;
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
            handleAppButtonClick(mousePos);
        }
    }

    switch (m_activeApp) {
        case ActiveApp::Map:
            m_map->handleEvent(event);
            break;
        case ActiveApp::Chatbot:
            m_chatbot->handleEvent(event);
            break;
        case ActiveApp::Database:
            m_database->handleEvent(event);
            break;
        case ActiveApp::Camera:
            m_camera->handleEvent(event);
            break;
        case ActiveApp::Settings:
            m_settings->handleEvent(event);
            break;
        default:
            break;
    }
}

void MainWindow::draw() {
    if (m_isPasswordProtected && !m_isPasswordEntered) {
        m_window.clear(sf::Color::White);
        m_window.draw(m_passwordPrompt);
        m_window.draw(m_passwordInput);
        return;
    }

    m_window.clear(sf::Color::White);

    updateTimeAndWeather();
    m_window.draw(m_timeText);
    m_window.draw(m_weatherText);

    drawAppButtons();

    switch (m_activeApp) {
        case ActiveApp::Map:
            m_map->draw();
            break;
        case ActiveApp::Chatbot:
            m_chatbot->draw();
            break;
        case ActiveApp::Database:
            m_database->draw();
            break;
        case ActiveApp::Camera:
            m_camera->draw();
            break;
        case ActiveApp::Settings:
            m_settings->draw();
            break;
        default:
            break;
    }
}

void MainWindow::updateTimeAndWeather() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    m_timeText.setString(ss.str());

    m_weatherText.setString(m_weather.getCurrentWeather());
}

void MainWindow::drawAppButtons() {
    for (const auto& button : m_appButtons) {
        m_window.draw(button);
    }
    for (const auto& text : m_appButtonTexts) {
        m_window.draw(text);
    }
}

void MainWindow::initializeAppButtons() {
    std::vector<std::string> appNames = {"Map", "Chatbot", "Database", "Camera", "Settings"};
    for (size_t i = 0; i < appNames.size(); ++i) {
        sf::RectangleShape button(sf::Vector2f(150, 50));
        button.setPosition(10, 100 + i * 60);
        button.setFillColor(sf::Color::White);
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color::Black);
        m_appButtons.push_back(button);

        sf::Text text(appNames[i], m_font, 20);
        text.setPosition(15, 110 + i * 60);
        text.setFillColor(sf::Color::Black);
        m_appButtonTexts.push_back(text);
    }
}

void MainWindow::handleAppButtonClick(sf::Vector2f mousePos) {
    for (size_t i = 0; i < m_appButtons.size(); ++i) {
        if (m_appButtons[i].getGlobalBounds().contains(mousePos)) {
            switchToApp(static_cast<ActiveApp>(i + 1));
            break;
        }
    }
}

void MainWindow::switchToApp(ActiveApp app) {
    m_activeApp = app;
}

void MainWindow::promptPassword() {
    m_isPasswordEntered = false;
    m_passwordInput.setString("");
}

void MainWindow::handlePasswordInput(const sf::Event& event) {
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 13) { // Enter key
            if (checkPassword(m_passwordInput.getString())) {
                m_isPasswordEntered = true;
            } else {
                m_passwordInput.setString("");
            }
        } else if (event.text.unicode == 8) { // Backspace
            std::string currentInput = m_passwordInput.getString();
            if (!currentInput.empty()) {
                currentInput.pop_back();
                m_passwordInput.setString(currentInput);
            }
        } else if (event.text.unicode < 128) {
            m_passwordInput.setString(m_passwordInput.getString() + static_cast<char>(event.text.unicode));
        }
    }
}

bool MainWindow::checkPassword(const std::string& input) {
    return input == m_password;
}

void MainWindow::onTimeChanged(const sf::Time& newTime) {
    // Update system time (this is just a placeholder, as changing system time requires system-level permissions)
    std::cout << "Time changed to: " << newTime.asSeconds() << " seconds since epoch" << std::endl;
}

void MainWindow::onDateChanged(const sf::Time& newDate) {
    // Update system date (this is just a placeholder, as changing system date requires system-level permissions)
    std::cout << "Date changed to: " << newDate.asSeconds() << " seconds since epoch" << std::endl;
}

void MainWindow::onMetricSystemChanged(bool isMetric) {
    std::cout << "Metric system " << (isMetric ? "enabled" : "disabled") << std::endl;
    // Update weather display and other relevant components
}

void MainWindow::onLowPowerModeChanged(bool isLowPower) {
    std::cout << "Low power mode " << (isLowPower ? "enabled" : "disabled") << std::endl;
    // Implement low power mode functionality
}

void MainWindow::onPasswordChanged(const std::string& newPassword) {
    m_password = newPassword;
    m_isPasswordProtected = !m_password.empty();
    if (m_isPasswordProtected) {
        promptPassword();
    }
}
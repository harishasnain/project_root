#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Chatbot {
public:
    Chatbot(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window);
    void resetShouldExit() { m_shouldExit = false; }
    bool shouldReturnToMain() const { return m_shouldExit; }

private:
    sf::RenderWindow& m_window;
    sf::Font m_font;
    sf::Text m_inputText;
    sf::RectangleShape m_inputBox;
    sf::RectangleShape m_exitButton;

    std::vector<sf::Text> m_chatHistory;

    void sendMessage();
    void addMessageToChatHistory(const std::string& message, bool isUser);
    bool m_shouldExit = false;
};
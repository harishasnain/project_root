#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Chatbot {
public:
    Chatbot(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void draw();

private:
    sf::RenderWindow& m_window;
    sf::Font m_font;
    sf::Text m_inputText;
    sf::RectangleShape m_inputBox;
    sf::RectangleShape m_exitButton;

    std::vector<sf::Text> m_chatHistory;

    void sendMessage();
    void addMessageToChatHistory(const std::string& message, bool isUser);
};
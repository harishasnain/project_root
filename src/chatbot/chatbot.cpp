#include "chatbot.hpp"
#include <iostream>

Chatbot::Chatbot(sf::RenderWindow& window)
    : m_window(window)
{
    if (!m_font.loadFromFile("resources/fonts/Roboto-Regular.ttf")) {
       // Handle font loading error
       std::cerr << "Failed to load font" << std::endl;
	}

    m_inputBox = sf::RectangleShape(sf::Vector2f(m_window.getSize().x - 20, 50));
    m_inputBox.setPosition(10, m_window.getSize().y - 60);
    m_inputBox.setFillColor(sf::Color::White);
    m_inputBox.setOutlineThickness(2);
    m_inputBox.setOutlineColor(sf::Color::Black);

    m_inputText.setFont(m_font);
    m_inputText.setCharacterSize(24);
    m_inputText.setFillColor(sf::Color::Black);
    m_inputText.setPosition(15, m_window.getSize().y - 55);

    m_exitButton = sf::RectangleShape(sf::Vector2f(50, 50));
    m_exitButton.setPosition(10, 10);
    m_exitButton.setFillColor(sf::Color::White);
    m_exitButton.setOutlineThickness(2);
    m_exitButton.setOutlineColor(sf::Color::Black);
}

void Chatbot::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

            if (m_exitButton.getGlobalBounds().contains(mousePos)) {
                // Exit chatbot app
                std::cout << "Exiting Chatbot" << std::endl;
            }
        }
    } else if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 13) { // Enter key
            sendMessage();
        } else if (event.text.unicode == 8) { // Backspace
            if (!m_inputText.getString().isEmpty()) {
                m_inputText.setString(m_inputText.getString().substring(0, m_inputText.getString().getSize() - 1));
            }
        } else if (event.text.unicode < 128) {
            m_inputText.setString(m_inputText.getString() + static_cast<char>(event.text.unicode));
        }
    }
}

void Chatbot::draw() {
    m_window.draw(m_inputBox);
    m_window.draw(m_inputText);
    m_window.draw(m_exitButton);

    for (const auto& message : m_chatHistory) {
        m_window.draw(message);
    }
}

void Chatbot::sendMessage() {
    std::string userMessage = m_inputText.getString();
    if (!userMessage.empty()) {
        addMessageToChatHistory(userMessage, true);
        m_inputText.setString("");

        // Chatbot response
        addMessageToChatHistory("Hello world!", false);
    }
}

void Chatbot::addMessageToChatHistory(const std::string& message, bool isUser) {
    sf::Text chatMessage;
    chatMessage.setFont(m_font);
    chatMessage.setCharacterSize(20);
    chatMessage.setFillColor(isUser ? sf::Color::Blue : sf::Color::Green);
    chatMessage.setString(isUser ? "You: " + message : "Chatbot: " + message);

    float yPos = m_window.getSize().y - 120 - (m_chatHistory.size() * 30);
    chatMessage.setPosition(15, yPos);

    m_chatHistory.push_back(chatMessage);

    // Limit chat history to last 10 messages
    if (m_chatHistory.size() > 10) {
        m_chatHistory.erase(m_chatHistory.begin());
    }

    // Adjust positions of existing messages
    for (size_t i = 0; i < m_chatHistory.size(); ++i) {
        m_chatHistory[i].setPosition(15, m_window.getSize().y - 120 - ((m_chatHistory.size() - i) * 30));
    }
}
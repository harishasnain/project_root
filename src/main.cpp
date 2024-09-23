#include <SFML/Graphics.hpp>
#include "mainwindow/mainwindow.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Multi-App Program");
    MainWindow mainWindow(window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            mainWindow.handleEvent(event);
        }

        window.clear(sf::Color::White);
        mainWindow.draw(window);
        window.display();
    }

    return 0;
}
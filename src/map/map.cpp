#include "map.hpp"
#include <iostream>

Map::Map(sf::RenderWindow& window)
    : m_window(window),
      m_isLayersPanelOpen(false),
      m_isSecondaryPanelOpen(false),
      m_isSearchActive(false),
      m_currentBaseLayer(BaseLayer::Streetmap)
{
    if (!m_font.loadFromFile("resources/fonts/Roboto-Regular.ttf")) {
       // Handle font loading error
       std::cerr << "Failed to load font" << std::endl;
	}

    // Initialize buttons and panels
    m_layersButton = sf::RectangleShape(sf::Vector2f(50, 50));
    m_layersButton.setPosition(m_window.getSize().x - 60, m_window.getSize().y - 60);
    m_layersButton.setFillColor(sf::Color::White);

    m_searchButton = sf::RectangleShape(sf::Vector2f(50, 50));
    m_searchButton.setPosition(10, m_window.getSize().y - 60);
    m_searchButton.setFillColor(sf::Color::White);

    m_exitButton = sf::RectangleShape(sf::Vector2f(50, 50));
    m_exitButton.setPosition(10, 10);
    m_exitButton.setFillColor(sf::Color::White);

    m_layersPanel = sf::RectangleShape(sf::Vector2f(m_window.getSize().x * 0.2f, m_window.getSize().y));
    m_layersPanel.setPosition(m_window.getSize().x, 0);
    m_layersPanel.setFillColor(sf::Color(200, 200, 200));

    m_secondaryPanel = sf::RectangleShape(sf::Vector2f(m_window.getSize().x * 0.2f, m_window.getSize().y));
    m_secondaryPanel.setPosition(-m_secondaryPanel.getSize().x, 0);
    m_secondaryPanel.setFillColor(sf::Color(200, 200, 200));

    m_searchBar = sf::RectangleShape(sf::Vector2f(m_window.getSize().x, 50));
    m_searchBar.setPosition(0, -50);
    m_searchBar.setFillColor(sf::Color::White);

    m_searchText.setFont(m_font);
    m_searchText.setCharacterSize(24);
    m_searchText.setFillColor(sf::Color::Black);
    m_searchText.setPosition(10, -40);

    // Initialize base layer buttons
    std::vector<std::string> baseLayerNames = {"Satellite", "Streetmap", "Terrain", "Topographic"};
    for (size_t i = 0; i < baseLayerNames.size(); ++i) {
        sf::RectangleShape button(sf::Vector2f(m_layersPanel.getSize().x - 20, 40));
        button.setPosition(m_layersPanel.getPosition().x + 10, 10 + i * 50);
        button.setFillColor(sf::Color::White);
        m_baseLayerButtons.push_back(button);
    }

    // Initialize secondary layer buttons
    std::vector<std::string> secondaryLayerNames = {"Information", "Resources"};
    for (size_t i = 0; i < secondaryLayerNames.size(); ++i) {
        sf::RectangleShape button(sf::Vector2f(m_secondaryPanel.getSize().x - 20, 40));
        button.setPosition(m_secondaryPanel.getPosition().x + 10, 10 + i * 50);
        button.setFillColor(sf::Color::White);
        m_secondaryLayerButtons.push_back(button);
    }

    // Initialize GDAL
    GDALAllRegister();

    // Load initial map data
    loadMapData("resources/maps/streetmap.gpkg");
}

Map::~Map() {
    GDALDestroyDriverManager();
}

void Map::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

            if (m_layersButton.getGlobalBounds().contains(mousePos)) {
                toggleLayersPanel();
            } else if (m_searchButton.getGlobalBounds().contains(mousePos)) {
                toggleSearch();
            } else if (m_exitButton.getGlobalBounds().contains(mousePos)) {
                // Exit map app
            }

            // Handle base layer button clicks
            for (size_t i = 0; i < m_baseLayerButtons.size(); ++i) {
                if (m_baseLayerButtons[i].getGlobalBounds().contains(mousePos)) {
                    changeBaseLayer(static_cast<BaseLayer>(i));
                    break;
                }
            }

            // Handle secondary layer button clicks
            for (size_t i = 0; i < m_secondaryLayerButtons.size(); ++i) {
                if (m_secondaryLayerButtons[i].getGlobalBounds().contains(mousePos)) {
                    addSecondaryLayer("Layer" + std::to_string(i + 1));
                    break;
                }
            }
        }
    } else if (event.type == sf::Event::TextEntered && m_isSearchActive) {
        if (event.text.unicode == 13) { // Enter key
            handleSearch();
        } else if (event.text.unicode == 8) { // Backspace
            if (!m_searchText.getString().isEmpty()) {
                m_searchText.setString(m_searchText.getString().substring(0, m_searchText.getString().getSize() - 1));
            }
        } else if (event.text.unicode < 128) {
            m_searchText.setString(m_searchText.getString() + static_cast<char>(event.text.unicode));
        }
    }
}

void Map::draw() {
    m_window.draw(m_mapSprite);
    m_window.draw(m_layersButton);
    m_window.draw(m_searchButton);
    m_window.draw(m_exitButton);

    if (m_isLayersPanelOpen) {
        m_window.draw(m_layersPanel);
        for (const auto& button : m_baseLayerButtons) {
            m_window.draw(button);
        }
    }

    if (m_isSecondaryPanelOpen) {
        m_window.draw(m_secondaryPanel);
        for (const auto& button : m_secondaryLayerButtons) {
            m_window.draw(button);
        }
    }

    if (m_isSearchActive) {
        m_window.draw(m_searchBar);
        m_window.draw(m_searchText);
    }
}

void Map::loadMapData(const std::string& filename) {
    m_currentDataset.reset(static_cast<GDALDataset*>(GDALOpen(filename.c_str(), GA_ReadOnly)));
    if (!m_currentDataset) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }

    renderMap();
}

void Map::renderMap() {
    // This is a simplified rendering process. In a real application, you'd need to handle
    // different projections, zoom levels, and more complex rendering logic.
    int width = m_currentDataset->GetRasterXSize();
    int height = m_currentDataset->GetRasterYSize();

    m_mapImage.create(width, height, sf::Color::White);

    // Read data from all bands
    std::vector<float> data(width * height * 3);
    m_currentDataset->RasterIO(GF_Read, 0, 0, width, height, data.data(), width, height, GDT_Float32, 3, nullptr, 0, 0, 0);

    // Convert the data to SFML image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            sf::Color color(
                static_cast<sf::Uint8>(data[index] * 255),
                static_cast<sf::Uint8>(data[index + 1] * 255),
                static_cast<sf::Uint8>(data[index + 2] * 255)
            );
            m_mapImage.setPixel(x, y, color);
        }
    }

    m_mapTexture.loadFromImage(m_mapImage);
    m_mapSprite.setTexture(m_mapTexture);

    updateMapView();
}

void Map::updateMapView() {
    // Adjust the map sprite to fit the window
    float scaleX = m_window.getSize().x / static_cast<float>(m_mapImage.getSize().x);
    float scaleY = m_window.getSize().y / static_cast<float>(m_mapImage.getSize().y);
    float scale = std::min(scaleX, scaleY);
    m_mapSprite.setScale(scale, scale);

    // Center the map in the window
    sf::FloatRect mapBounds = m_mapSprite.getGlobalBounds();
    m_mapSprite.setPosition(
        (m_window.getSize().x - mapBounds.width) / 2.f,
        (m_window.getSize().y - mapBounds.height) / 2.f
    );
}

void Map::toggleLayersPanel() {
    m_isLayersPanelOpen = !m_isLayersPanelOpen;
    if (m_isLayersPanelOpen) {
        m_layersPanel.setPosition(m_window.getSize().x - m_layersPanel.getSize().x, 0);
        m_isSecondaryPanelOpen = false;
        m_secondaryPanel.setPosition(-m_secondaryPanel.getSize().x, 0);
    } else {
        m_layersPanel.setPosition(m_window.getSize().x, 0);
    }
}

void Map::toggleSecondaryPanel() {
    m_isSecondaryPanelOpen = !m_isSecondaryPanelOpen;
    if (m_isSecondaryPanelOpen) {
        m_secondaryPanel.setPosition(0, 0);
        m_isLayersPanelOpen = false;
        m_layersPanel.setPosition(m_window.getSize().x, 0);
    } else {
        m_secondaryPanel.setPosition(-m_secondaryPanel.getSize().x, 0);
    }
}

void Map::toggleSearch() {
    m_isSearchActive = !m_isSearchActive;
    if (m_isSearchActive) {
        m_searchBar.setPosition(0, 0);
        m_searchText.setPosition(10, 10);
    } else {
        m_searchBar.setPosition(0, -50);
        m_searchText.setPosition(10, -40);
        m_searchText.setString("");
    }
}

void Map::handleSearch() {
    std::string searchQuery = m_searchText.getString();
    // Implement search functionality here
    std::cout << "Searching for: " << searchQuery << std::endl;
    toggleSearch();
}

void Map::changeBaseLayer(BaseLayer layer) {
    m_currentBaseLayer = layer;
    std::string filename;
    switch (layer) {
        case BaseLayer::Satellite:
            filename = "resources/maps/satellite.gpkg";
            break;
        case BaseLayer::Streetmap:
            filename = "resources/maps/streetmap.gpkg";
            break;
        case BaseLayer::Terrain:
            filename = "resources/maps/terrain.gpkg";
            break;
        case BaseLayer::Topographic:
            filename = "resources/maps/topographic.gpkg";
            break;
    }
    loadMapData(filename);
}

void Map::addSecondaryLayer(const std::string& layerName) {
    // Implement secondary layer addition here
    std::cout << "Adding secondary layer: " << layerName << std::endl;
}
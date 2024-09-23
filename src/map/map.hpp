#pragma once

#include <SFML/Graphics.hpp>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <string>
#include <vector>
#include <memory>

class Map {
public:
    Map(sf::RenderWindow& window);
    ~Map();

    void handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window);
    void resetShouldExit() { m_shouldExit = false; }
    void setNeedsRedraw();
    bool needsRedraw() const;
    bool shouldReturnToMain() const { return m_shouldExit; }

private:
    sf::RenderWindow& m_window;
    sf::Font m_font;
    sf::Text m_searchText;
    sf::RectangleShape m_searchBar;
    sf::RectangleShape m_layersButton;
    sf::RectangleShape m_searchButton;
    sf::RectangleShape m_exitButton;

    bool m_isLayersPanelOpen;
    bool m_isSecondaryPanelOpen;
    bool m_isSearchActive;
    bool m_shouldExit = false;
    bool m_needsRedraw;

    sf::RectangleShape m_layersPanel;
    sf::RectangleShape m_secondaryPanel;

    std::vector<sf::RectangleShape> m_baseLayerButtons;
    std::vector<sf::RectangleShape> m_secondaryLayerButtons;
    std::vector<sf::VertexArray> m_vectorShapes; // Store vector shapes

    enum class BaseLayer {
        Satellite,
        Streetmap,
        Terrain,
        Topographic
    };

    BaseLayer m_currentBaseLayer;

    std::unique_ptr<GDALDataset> m_currentDataset;
    std::vector<std::string> m_secondaryLayerNames;
    sf::Image m_mapImage;
    sf::Texture m_mapTexture;
    sf::Sprite m_mapSprite;

    void loadMapData(const std::string& filename);
    void loadVectorData(GDALDataset* dataset); // Method to load vector data
    void renderMap();
    void updateMapView();
    void toggleLayersPanel();
    void toggleSecondaryPanel();
    void toggleSearch();
    void handleSearch();
    void changeBaseLayer(BaseLayer layer);
    void addSecondaryLayer(const std::string& layerName);
    void processGeometry(OGRGeometry* geom, OGRCoordinateTransformation* coordTransform);
};
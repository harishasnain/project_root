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
    void draw();

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

    sf::RectangleShape m_layersPanel;
    sf::RectangleShape m_secondaryPanel;

    std::vector<sf::RectangleShape> m_baseLayerButtons;
    std::vector<sf::RectangleShape> m_secondaryLayerButtons;

    enum class BaseLayer {
        Satellite,
        Streetmap,
        Terrain,
        Topographic
    };

    BaseLayer m_currentBaseLayer;

    std::unique_ptr<GDALDataset> m_currentDataset;
    sf::Image m_mapImage;
    sf::Texture m_mapTexture;
    sf::Sprite m_mapSprite;

    void loadMapData(const std::string& filename);
    void renderMap();
    void updateMapView();
    void toggleLayersPanel();
    void toggleSecondaryPanel();
    void toggleSearch();
    void handleSearch();
    void changeBaseLayer(BaseLayer layer);
    void addSecondaryLayer(const std::string& layerName);
};
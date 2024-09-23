#include "map.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <ogr_geometry.h>

Map::Map(sf::RenderWindow& window)
    : m_window(window),
      m_isLayersPanelOpen(false),
      m_isSecondaryPanelOpen(false),
      m_isSearchActive(false),
      m_currentBaseLayer(BaseLayer::Streetmap),
      m_needsRedraw(true)
{

    // Set the PROJ_LIB environment variable
    _putenv("PROJ_LIB=C:\\project_root\\vcpkg\\installed\\x64-windows\\share\\proj");

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
    m_secondaryLayerNames = {"Information", "Resources"}; // Initialize secondary layer names
    for (size_t i = 0; i < m_secondaryLayerNames.size(); ++i) {
        sf::RectangleShape button(sf::Vector2f(m_secondaryPanel.getSize().x - 20, 40));
        button.setPosition(m_secondaryPanel.getPosition().x + 10, 10 + i * 50);
        button.setFillColor(sf::Color::White);
        m_secondaryLayerButtons.push_back(button);
    }

    // Initialize GDAL
    GDALAllRegister();

    // Load initial map data
    loadMapData("resources/maps/streetmap.gpkg");
    setNeedsRedraw();
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
                setNeedsRedraw();
            } else if (m_searchButton.getGlobalBounds().contains(mousePos)) {
                toggleSearch();
                setNeedsRedraw();
            } else if (m_exitButton.getGlobalBounds().contains(mousePos)) {
                m_shouldExit = true;
            }

            // Handle base layer button clicks
            for (size_t i = 0; i < m_baseLayerButtons.size(); ++i) {
                if (m_baseLayerButtons[i].getGlobalBounds().contains(mousePos)) {
                    changeBaseLayer(static_cast<BaseLayer>(i));
                    setNeedsRedraw();
                    break;
                }
            }

            // Handle secondary layer button clicks
            for (size_t i = 0; i < m_secondaryLayerButtons.size(); ++i) {
                if (m_secondaryLayerButtons[i].getGlobalBounds().contains(mousePos)) {
                    addSecondaryLayer(m_secondaryLayerNames[i]);
                    setNeedsRedraw();
                    break;
                }
            }
        }
    } else if (event.type == sf::Event::TextEntered) {
        if (m_isSearchActive) {
            if (event.text.unicode == '\b') {
                // Handle backspace
                if (!m_searchText.getString().isEmpty()) {
                    std::string str = m_searchText.getString();
                    str.pop_back();
                    m_searchText.setString(str);
                    setNeedsRedraw();
                }
            } else if (event.text.unicode == '\r') {
                // Handle enter
                handleSearch();
                setNeedsRedraw();
            } else {
                // Handle regular text input
                m_searchText.setString(m_searchText.getString() + static_cast<char>(event.text.unicode));
                setNeedsRedraw();
            }
        }
    }
}

void Map::draw(sf::RenderWindow& window) {
    if (!m_needsRedraw) return;

    window.clear(); // Clear the window before drawing

    // Draw the map sprite if it exists
    if (m_mapTexture.getSize().x > 0 && m_mapTexture.getSize().y > 0) {
        window.draw(m_mapSprite);
    }

    window.draw(m_layersButton);
    window.draw(m_searchButton);
    window.draw(m_exitButton);

    if (m_isLayersPanelOpen) {
        window.draw(m_layersPanel);
        for (const auto& button : m_baseLayerButtons) {
            window.draw(button);
        }
    }

    if (m_isSecondaryPanelOpen) {
        window.draw(m_secondaryPanel);
        for (const auto& button : m_secondaryLayerButtons) {
            window.draw(button);
        }
    }

    if (m_isSearchActive) {
        window.draw(m_searchBar);
        window.draw(m_searchText);
    }

    // Draw vector shapes
    for (const auto& shape : m_vectorShapes) {
        window.draw(shape);
    }

    window.display(); // Display the window contents
    m_needsRedraw = false;
}

void Map::setNeedsRedraw() {
    m_needsRedraw = true;
}

bool Map::needsRedraw() const {
    return m_needsRedraw;
}

void Map::loadMapData(const std::string& filename) {
    std::cout << "Loading map data from: " << filename << std::endl;
    m_currentDataset.reset(static_cast<GDALDataset*>(GDALOpenEx(filename.c_str(), GDAL_OF_VECTOR | GDAL_OF_RASTER, nullptr, nullptr, nullptr)));
    if (!m_currentDataset) {
        std::cerr << "Failed to load map data from " << filename << std::endl;
        return;
    }

    // Load raster data
    GDALRasterBand* band = m_currentDataset->GetRasterBand(1);
    if (band) {
        int width = band->GetXSize();
        int height = band->GetYSize();
        std::vector<float> data(width * height);
        band->RasterIO(GF_Read, 0, 0, width, height, data.data(), width, height, GDT_Float32, 0, 0);

        m_mapImage.create(width, height);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float value = data[y * width + x];
                sf::Color color = sf::Color::White; // Default color
                if (value > 0) {
                    color = sf::Color::Black; // Example: change color based on value
                }
                m_mapImage.setPixel(x, y, color);
            }
        }

        m_mapTexture.loadFromImage(m_mapImage);
        m_mapSprite.setTexture(m_mapTexture);
        std::cout << "Raster data loaded successfully." << std::endl;
    } else {
        std::cerr << "No raster band found in the dataset." << std::endl;
    }

    // Load vector data
    loadVectorData(m_currentDataset.get());

    updateMapView();
}


void Map::loadVectorData(GDALDataset* dataset) {
    m_vectorShapes.clear();
    std::cout << "Loading vector data..." << std::endl;

    // Get the spatial reference of the dataset
    OGRSpatialReference* datasetSRS = nullptr;
    if (dataset->GetLayerCount() > 0) {
        datasetSRS = dataset->GetLayer(0)->GetSpatialRef();
    }

    // Create a transformation to the window's coordinate system
    OGRSpatialReference windowSRS;
    windowSRS.SetWellKnownGeogCS("WGS84");
    OGRCoordinateTransformation* coordTransform = nullptr;
    if (datasetSRS) {
        coordTransform = OGRCreateCoordinateTransformation(datasetSRS, &windowSRS);
    }

    for (int i = 0; i < dataset->GetLayerCount(); ++i) {
        OGRLayer* layer = dataset->GetLayer(i);
        if (!layer) {
            std::cout << "Layer " << i << " is null" << std::endl;
            continue;
        }

        std::cout << "Processing layer " << i << std::endl;

        layer->ResetReading();
        OGRFeature* feature;
        while ((feature = layer->GetNextFeature()) != nullptr) {
            OGRGeometry* geom = feature->GetGeometryRef();
            if (!geom) {
                OGRFeature::DestroyFeature(feature);
                continue;
            }

            processGeometry(geom, coordTransform);
            OGRFeature::DestroyFeature(feature);
        }
    }

    if (coordTransform) {
        OCTDestroyCoordinateTransformation(coordTransform);
    }

    std::cout << "Vector data loaded successfully." << std::endl;
}

void Map::processGeometry(OGRGeometry* geom, OGRCoordinateTransformation* coordTransform) {
    OGRwkbGeometryType type = wkbFlatten(geom->getGeometryType());
    sf::VertexArray shape;

    if (type == wkbLineString || type == wkbLinearRing || type == wkbCircularString) {
        shape = sf::VertexArray(sf::LineStrip);
        OGRSimpleCurve* curve = dynamic_cast<OGRSimpleCurve*>(geom);
        if (curve) {
            int numPoints = curve->getNumPoints();
            for (int j = 0; j < numPoints; ++j) {
                OGRPoint point;
                curve->getPoint(j, &point);
                double x = point.getX(), y = point.getY();
                if (coordTransform) coordTransform->Transform(1, &x, &y);
                double scaledX = (x + 180.0) * (m_window.getSize().x / 360.0);
                double scaledY = (90.0 - y) * (m_window.getSize().y / 180.0);
                shape.append(sf::Vertex(sf::Vector2f(scaledX, scaledY), sf::Color::Red));
            }

            // For CircularString, add more points to smooth the curve
            if (type == wkbCircularString) {
                sf::VertexArray smoothedShape(sf::LineStrip);
                int smoothPoints = 100;
                for (int i = 0; i < shape.getVertexCount() - 1; ++i) {
                    sf::Vector2f p1 = shape[i].position;
                    sf::Vector2f p2 = shape[i + 1].position;
                    for (int j = 0; j < smoothPoints; ++j) {
                        float t = j / static_cast<float>(smoothPoints);
                        sf::Vector2f interpolated = p1 + (p2 - p1) * t;
                        smoothedShape.append(sf::Vertex(interpolated, sf::Color::Red));
                    }
                }
                shape = smoothedShape;
            }

            m_vectorShapes.push_back(shape);
        }
    } else if (type == wkbPolygon) {
        OGRPolygon* poly = dynamic_cast<OGRPolygon*>(geom);
        if (poly) {
            processGeometry(poly->getExteriorRing(), coordTransform);
            for (int r = 0; r < poly->getNumInteriorRings(); ++r) {
                processGeometry(poly->getInteriorRing(r), coordTransform);
            }
        }
    } else if (type == wkbMultiLineString || type == wkbMultiPolygon || type == wkbGeometryCollection) {
        OGRGeometryCollection* collection = dynamic_cast<OGRGeometryCollection*>(geom);
        if (collection) {
            for (int j = 0; j < collection->getNumGeometries(); ++j) {
                processGeometry(collection->getGeometryRef(j), coordTransform);
            }
        }
    } else if (type == wkbCompoundCurve) {
        OGRCompoundCurve* compound = dynamic_cast<OGRCompoundCurve*>(geom);
        if (compound) {
            for (int j = 0; j < compound->getNumCurves(); ++j) {
                processGeometry(compound->getCurve(j), coordTransform);
            }
        }
    } else {
        std::cout << "Unsupported geometry type: " << OGRGeometryTypeToName(type) << std::endl;
    }
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
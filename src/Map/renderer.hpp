#ifndef MAP_RENDERER_HPP
#define MAP_RENDERER_HPP

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <future>
#include <random>
#include <iostream>
#include "../Utils/progressbar.hpp"
#include "map_texture.hpp"
#define DEBUG_MAP_RENDERER

using json = nlohmann::json;

struct RendererSettings {
    sf::Vector2f scale;
    sf::Vector2f offset;

    // Names
    float fontSize = 10.0f;
    float fontColor[3] = {1.0f, 1.0f, 1.0f};

};

struct Coordinate {
    double x, y;
};

struct Polygon {
    std::vector<Coordinate> coordinates;
};

class MapRenderer {
private:
    std::string selectedCountry;
    Polygon selectedPolygon;
    std::vector<std::string> names;
    std::vector<Polygon> polygons;
    Coordinate minBounds, maxBounds;
    double scale;
    sf::Vector2f offset;
    std::vector<sf::Color> colors;
    std::string filename;
    sf::Font font;

    void calculateScaleAndOffset(const sf::Vector2u& windowSize, float zoomFactor, const sf::Vector2u& textureSize);
    bool isPointInPolygon(const sf::Vector2f& point, const std::vector<Coordinate>& polygon);
    sf::Vector2f calculateCentroid(const std::vector<Coordinate>& coordinates);

public:
    bool toggleNames = false;
    MapRenderer(const std::string& filename, ProgressBar& progressBar);

    void calculateBounds();
    void loadFromGeoJSON();

    void addPolygon(const json& coordinates);

    void draw(sf::RenderWindow& window, float zoomFactor, const RendererSettings& rendererSettings, const sf::Vector2u& textureSize);
    void update(const sf::Vector2f& mousePos, sf::RenderWindow& window, MapDrawTexture& mapDrawTexture);
    void updateSelectedColor(const sf::Color& color);
};


#endif // MAP_RENDERER_HPP

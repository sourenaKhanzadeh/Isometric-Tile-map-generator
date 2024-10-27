#ifndef MAP_RENDERER_HPP
#define MAP_RENDERER_HPP

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>

using json = nlohmann::json;

struct Coordinate {
    double x, y;
};

struct Polygon {
    std::vector<Coordinate> coordinates;
};

class MapRenderer {
private:
    std::vector<Polygon> polygons;
    Coordinate minBounds, maxBounds;
    double scale;
    sf::Vector2f offset;
    std::vector<sf::Color> colors;

    void calculateBounds();

    void calculateScaleAndOffset(const sf::Vector2u& windowSize);
    void generateColors();

public:
    MapRenderer(const std::string& filename);

    void loadFromGeoJSON(const std::string& filename);

    void addPolygon(const json& coordinates);

    void draw(sf::RenderWindow& window);
};


#endif // MAP_RENDERER_HPP

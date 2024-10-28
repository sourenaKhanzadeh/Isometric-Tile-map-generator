#ifndef MAP_RENDERER_HPP
#define MAP_RENDERER_HPP

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>

#define DEBUG_MAP_RENDERER

using json = nlohmann::json;

struct Coordinate {
    double x, y;
};

struct Polygon {
    std::vector<Coordinate> coordinates;
};

class MapRenderer {
private:
    std::vector<std::string> names;
    std::vector<Polygon> polygons;
    Coordinate minBounds, maxBounds;
    double scale;
    sf::Vector2f offset;
    std::vector<sf::Color> colors;
    std::string filename;


    void calculateScaleAndOffset(const sf::Vector2u& windowSize);
    bool isPointInPolygon(const sf::Vector2f& point, const std::vector<Coordinate>& polygon);

public:
    MapRenderer(const std::string& filename);

    void calculateBounds();
    void generateColors();
    void loadFromGeoJSON();

    void addPolygon(const json& coordinates);

    void draw(sf::RenderWindow& window);
    void update(const sf::Vector2f& mousePos);
};


#endif // MAP_RENDERER_HPP

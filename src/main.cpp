#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

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

    void calculateBounds() {
        for (const auto& polygon : polygons) {
            for (const auto& coord : polygon.coordinates) {
                minBounds.x = std::min(minBounds.x, coord.x);
                maxBounds.x = std::max(maxBounds.x, coord.x);
                minBounds.y = std::min(minBounds.y, coord.y);
                maxBounds.y = std::max(maxBounds.y, coord.y);
            }
        }
    }

    void calculateScaleAndOffset(const sf::Vector2u& windowSize) {
        double scaleX = windowSize.x / (maxBounds.x - minBounds.x);
        double scaleY = windowSize.y / (maxBounds.y - minBounds.y);
        scale = std::min(scaleX, scaleY);

        double mapWidth = (maxBounds.x - minBounds.x) * scale;
        double mapHeight = (maxBounds.y - minBounds.y) * scale;

        offset.x = (windowSize.x - mapWidth) / 2.0f;
        offset.y = (windowSize.y - mapHeight) / 2.0f;
    }

public:
    MapRenderer(const std::string& filename) {
        loadFromGeoJSON(filename);
        calculateBounds();
    }

    void loadFromGeoJSON(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open GeoJSON file!");
        }

        json geojsonData;
        file >> geojsonData;

        for (const auto& feature : geojsonData["features"]) {
            const auto& geometry = feature["geometry"];
            std::string geomType = geometry["type"];

            if (geomType == "Polygon") {
                addPolygon(geometry["coordinates"]);
            } else if (geomType == "MultiPolygon") {
                for (const auto& polygon : geometry["coordinates"]) {
                    addPolygon(polygon);
                }
            }
        }
    }

    void addPolygon(const json& coordinates) {
        Polygon polygon;
        for (const auto& point : coordinates[0]) {
            polygon.coordinates.push_back({point[0].get<double>(), point[1].get<double>()});
        }
        polygons.push_back(polygon);
    }

    void draw(sf::RenderWindow& window) {
        calculateScaleAndOffset(window.getSize());

        for (const auto& polygon : polygons) {
            sf::ConvexShape shape;
            shape.setPointCount(polygon.coordinates.size());

            for (size_t i = 0; i < polygon.coordinates.size(); ++i) {
                float x = static_cast<float>((polygon.coordinates[i].x - minBounds.x) * scale + offset.x);
                float y = static_cast<float>((maxBounds.y - polygon.coordinates[i].y) * scale + offset.y);
                shape.setPoint(i, sf::Vector2f(x, y));
            }

            shape.setFillColor(sf::Color::Green);
            window.draw(shape);
        }
    }
};

class CameraController {
private:
    sf::View& view;
    float moveSpeed;
    float zoomFactor;

public:
    CameraController(sf::View& view, float moveSpeed = 10.0f, float zoomFactor = 1.1f)
        : view(view), moveSpeed(moveSpeed), zoomFactor(zoomFactor) {}

    void handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.delta > 0) {
                view.zoom(1.0f / zoomFactor);
            } else if (event.mouseWheelScroll.delta < 0) {
                view.zoom(zoomFactor);
            }
        }
    }

    void update() {
        float zoomLevel = view.getSize().x / 1920.0f; // Assuming 1920 is the initial window width

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            view.move(-moveSpeed * zoomLevel, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            view.move(moveSpeed * zoomLevel, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            view.move(0, -moveSpeed * zoomLevel);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            view.move(0, moveSpeed * zoomLevel);
        }
    }
};

int main() {
    sf::RenderWindow window({1920u, 1080u}, "Fortifier: Forge and Conquer");
    window.setFramerateLimit(144);

    sf::View view(sf::FloatRect(0, 0, 1920, 1080));
    window.setView(view);

    MapRenderer mapRenderer("./countries.geo.json");
    CameraController cameraController(view);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            cameraController.handleEvent(event);
        }

        cameraController.update();
        window.setView(view);

        window.clear();
        mapRenderer.draw(window);
        window.display();
    }

    return 0;
}

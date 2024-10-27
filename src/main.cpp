#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

using json = nlohmann::json;

void drawPolygon(sf::RenderWindow& window, const std::vector<sf::Vector2f>& coordinates) {
    sf::ConvexShape polygon;
    polygon.setPointCount(coordinates.size());

    for (size_t i = 0; i < coordinates.size(); ++i) {
        polygon.setPoint(i, coordinates[i]);
    }

    polygon.setFillColor(sf::Color::Green);
    window.draw(polygon);
}

int main() {
    std::ifstream geojsonFile("./countries.geo.json");
    if (!geojsonFile.is_open()) {
        std::cerr << "Failed to open GeoJSON file!" << std::endl;
        return -1;
    }

    json geojsonData;
    geojsonFile >> geojsonData;

    sf::RenderWindow window({1920u, 1080u}, "Fortifier: Forge and Conquer");
    window.setFramerateLimit(144);

    // Variables for min and max coordinates
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    // Store all the polygons
    std::vector<std::vector<std::pair<double, double>>> allPolygons;

    // Iterate through features in GeoJSON
    for (const auto& feature : geojsonData["features"]) {
        const auto& geometry = feature["geometry"];
        std::string geomType = geometry["type"];

        if (geomType == "Polygon") {
            // Handle Polygon geometry
            for (const auto& coords : geometry["coordinates"]) {
                std::vector<std::pair<double, double>> coordinates;

                for (const auto& point : coords) {
                    double x = point[0].get<double>(); // longitude
                    double y = point[1].get<double>(); // latitude

                    // Update min and max for scaling
                    minX = std::min(minX, x);
                    maxX = std::max(maxX, x);
                    minY = std::min(minY, y);
                    maxY = std::max(maxY, y);

                    coordinates.emplace_back(x, y);
                }
                allPolygons.push_back(coordinates);
            }
        } else if (geomType == "MultiPolygon") {
            // Handle MultiPolygon geometry
            for (const auto& polygon : geometry["coordinates"]) {
                for (const auto& coords : polygon) {
                    std::vector<std::pair<double, double>> coordinates;

                    for (const auto& point : coords) {
                        double x = point[0].get<double>(); // longitude
                        double y = point[1].get<double>(); // latitude

                        // Update min and max for scaling
                        minX = std::min(minX, x);
                        maxX = std::max(maxX, x);
                        minY = std::min(minY, y);
                        maxY = std::max(maxY, y);

                        coordinates.emplace_back(x, y);
                    }
                    allPolygons.push_back(coordinates);
                }
            }
        }
    }

    // Calculate scaling factors to fit the map within the window
    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;

    double scaleX = windowWidth / (maxX - minX);
    double scaleY = windowHeight / (maxY - minY);
    double scale = std::min(scaleX, scaleY); // Uniform scaling to maintain aspect ratio

    // Calculate offsets to center the map
    double mapWidth = (maxX - minX) * scale;
    double mapHeight = (maxY - minY) * scale;

    double offsetX = (windowWidth - mapWidth) / 2.0;
    double offsetY = (windowHeight - mapHeight) / 2.0;

    // Initialize the view
    sf::View view(sf::FloatRect(0, 0, windowWidth, windowHeight));
    view.setSize(windowWidth, windowHeight);

    float moveSpeed = 10.0f; // Camera movement speed
    float zoomFactor = 1.1f; // Zoom factor

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Mouse wheel event for zooming
            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0) {
                    view.zoom(1.0f / zoomFactor); // Zoom in
                } else if (event.mouseWheelScroll.delta < 0) {
                    view.zoom(zoomFactor); // Zoom out
                }
            }
        }

        // Camera movement with arrow keys or WASD
        float zoomLevel = view.getSize().x / windowWidth; // Current zoom level

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

        // Apply the view to the window
        window.setView(view);

        window.clear();

        // Draw all polygons with transformed coordinates
        for (const auto& coordinates : allPolygons) {
            std::vector<sf::Vector2f> transformedCoordinates;

            for (const auto& coord : coordinates) {
                // Scale and translate coordinates
                double x = (coord.first - minX) * scale + offsetX;
                double y = (maxY - coord.second) * scale + offsetY; // Flip Y-axis

                transformedCoordinates.emplace_back(static_cast<float>(x), static_cast<float>(y));
            }

            drawPolygon(window, transformedCoordinates);
        }

        window.display();
    }

    return 0;
}

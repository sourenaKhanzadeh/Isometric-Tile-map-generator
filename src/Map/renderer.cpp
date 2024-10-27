#include "renderer.hpp"


void MapRenderer::calculateBounds() {
    for (const auto& polygon : polygons) {
        for (const auto& coord : polygon.coordinates) {
            minBounds.x = std::min(minBounds.x, coord.x);
            maxBounds.x = std::max(maxBounds.x, coord.x);
            minBounds.y = std::min(minBounds.y, coord.y);
            maxBounds.y = std::max(maxBounds.y, coord.y);
        }
    }
}

void MapRenderer::calculateScaleAndOffset(const sf::Vector2u& windowSize) {
    double scaleX = windowSize.x / (maxBounds.x - minBounds.x);
    double scaleY = windowSize.y / (maxBounds.y - minBounds.y);
    scale = std::min(scaleX, scaleY);

    double mapWidth = (maxBounds.x - minBounds.x) * scale;
    double mapHeight = (maxBounds.y - minBounds.y) * scale;

    offset.x = (windowSize.x - mapWidth) / 2.0f;
    offset.y = (windowSize.y - mapHeight) / 2.0f;
}

void MapRenderer::generateColors() {
    for (int i = 0; i < polygons.size(); ++i) {
        // make it midtone light gray
        colors.push_back(sf::Color(128, 128, 128));
    }
}

MapRenderer::MapRenderer(const std::string& filename) : filename(filename) {
}

void MapRenderer::loadFromGeoJSON() {
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

void MapRenderer::addPolygon(const json& coordinates) {
    Polygon polygon;
    for (const auto& point : coordinates[0]) {
        polygon.coordinates.push_back({point[0].get<double>(), point[1].get<double>()});
    }
    polygons.push_back(polygon);
}

void MapRenderer::draw(sf::RenderWindow& window) {
    calculateScaleAndOffset(window.getSize());

    for (int i = 0; i < polygons.size(); ++i) {
        const auto& polygon = polygons[i];
        sf::ConvexShape shape;
        shape.setPointCount(polygon.coordinates.size());

        for (size_t j = 0; j < polygon.coordinates.size(); ++j) {
            float x = static_cast<float>((polygon.coordinates[j].x - minBounds.x) * scale + offset.x);
            float y = static_cast<float>((maxBounds.y - polygon.coordinates[j].y) * scale + offset.y);
            shape.setPoint(j, sf::Vector2f(x, y));
        }

        shape.setFillColor(colors[i]);
        window.draw(shape);
    }
}

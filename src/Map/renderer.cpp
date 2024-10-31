#include "renderer.hpp"

void MapRenderer::calculateBounds() {
    if (polygons.empty()) return;
    
    // Initialize bounds with first coordinate
    const auto& firstPolygon = polygons[0];
    const auto& firstPoint = firstPolygon[0];
    minBounds = sf::Vector2f(firstPoint.position.x, firstPoint.position.y);
    maxBounds = sf::Vector2f(firstPoint.position.x, firstPoint.position.y);
    
    // Single pass bounds calculation
    #pragma omp parallel for reduction(min:minBounds.x,minBounds.y) reduction(max:maxBounds.x,maxBounds.y)
    for (size_t i = 0; i < polygons.size(); i++) {
        for (size_t j = 0; j < polygons[i].getVertexCount(); j++) {
            const auto& coord = polygons[i][j];
            minBounds.x = std::min(minBounds.x, static_cast<decltype(minBounds.x)>(coord.position.x));
            maxBounds.x = std::max(maxBounds.x, static_cast<decltype(maxBounds.x)>(coord.position.x));
            minBounds.y = std::min(minBounds.y, static_cast<decltype(minBounds.y)>(coord.position.y));
            maxBounds.y = std::max(maxBounds.y, static_cast<decltype(maxBounds.y)>(coord.position.y));
        }
    }
}

void MapRenderer::calculateScaleAndOffset(const sf::Vector2u& windowSize, float zoomFactor, const sf::Vector2u& textureSize) {
    // Calculate the width and height of the polygon bounds in coordinate space
    float mapCoordWidth = maxBounds.x - minBounds.x;
    float mapCoordHeight = maxBounds.y - minBounds.y;

    // Calculate scale to fit the window dimensions
    float scaleX = static_cast<float>(windowSize.x) / mapCoordWidth;
    float scaleY = static_cast<float>(windowSize.y) / mapCoordHeight;

    // Apply the zoom factor, keeping the aspect ratio consistent
    scale = std::min(scaleX, scaleY) * zoomFactor;

    // Calculate offset to align and center polygons within the window
    offset.x = (windowSize.x - mapCoordWidth * scale) / 2.0f;
    offset.y = (windowSize.y - mapCoordHeight * scale) / 2.0f;
}



MapRenderer::MapRenderer(const std::string& filename, ProgressBar& progressBar) : filename(filename) {
    // Reserve space for large datasets
    colors.reserve(1000);
    
    font.loadFromFile("res/font/arial.TTF");

    // async load the geojson file
    std::thread loadThread([this, &progressBar]() {
        loadFromGeoJSON();
        progressBar.incrementProgress();
    });
    loadThread.join();
}

void MapRenderer::loadFromGeoJSON() {
    // Random colors
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open GeoJSON file!");
    }

    json geojsonData = json::parse(file);

    if (!geojsonData.is_object() || !geojsonData.contains("features")) {
        throw std::runtime_error("Invalid GeoJSON structure!");
    }

    const auto& features = geojsonData["features"];
    polygons.reserve(features.size() * 2); // Reserve estimated space
    colors.reserve(features.size() * 2);

    #pragma omp parallel for
    for (size_t i = 0; i < features.size(); i++) {
        const auto& feature = features[i];
        std::string name;

        // Check if the "name" field exists; if not, assign a default name
        if (feature.contains("properties") && feature["properties"].contains("name")) {
            name = feature["properties"]["name"].get<std::string>();
        } else {
            name = "Unknown_" + std::to_string(i); // Assign a unique placeholder name
        }

        const auto& geometry = feature["geometry"];
        const std::string& geomType = geometry["type"];

        if (geomType == "Polygon") {
            #pragma omp critical
            addPolygon(geometry["coordinates"]);
        } else if (geomType == "MultiPolygon") {
            for (const auto& polygon : geometry["coordinates"]) {
                #pragma omp critical
                addPolygon(polygon);
                colors.push_back(sf::Color(dis(gen), dis(gen), dis(gen), 50));
                names.push_back(name);
            }
        }
    }
}

void MapRenderer::addPolygon(const json& coordinates) {
    sf::ConvexShape polygon;
    const auto& points = coordinates[0];
    polygon.setPointCount(points.size());
    
    for (size_t i = 0; i < points.size(); i++) {
        polygon.setPoint(i, sf::Vector2f(
            points[i][0].get<double>(),
            points[i][1].get<double>()
        ));
    }
    // Convert sf::ConvexShape to sf::VertexArray
    sf::VertexArray vertexArray(sf::TrianglesFan, polygon.getPointCount());
    for (size_t i = 0; i < polygon.getPointCount(); ++i) {
        vertexArray[i].position = polygon.getPoint(i);
        vertexArray[i].color = sf::Color::White; // or any default color
    }

    // make the last point the same as the first point to close the shape
    vertexArray[vertexArray.getVertexCount() - 1].position = vertexArray[0].position;
    vertexArray[vertexArray.getVertexCount() - 1].color = vertexArray[0].color;
    polygons.push_back(std::move(vertexArray));
}

sf::Vector2f MapRenderer::calculateCentroid(std::vector<sf::Vector2f>& coordinates) {
    float sumX = 0.0f;
    float sumY = 0.0f;
    for (const auto& coord : coordinates) {
        sumX += coord.x;
        sumY += coord.y;
    }
    float centerX = sumX / coordinates.size();
    float centerY = sumY / coordinates.size();
    return sf::Vector2f(centerX, centerY);
}



void MapRenderer::draw(sf::RenderWindow& window, float zoomFactor, const RendererSettings& rendererSettings, const sf::Vector2u& textureSize) {
    calculateScaleAndOffset(window.getSize(), zoomFactor, textureSize);

    // Create vertex arrays for batch rendering
    sf::VertexArray vertexArray(sf::Triangles);

    // Initialize countryNames only if toggleNames is true
    std::vector<sf::Text> countryNames;
    if (toggleNames) {
        countryNames.resize(polygons.size());
    }

    // Loop through each polygon and add vertices to a single vertex array
    for (size_t i = 0; i < polygons.size(); i++) {
        const auto& polygon = polygons[i];
        const sf::Color color = colors[i];

        if (toggleNames) {
            // Check if name is valid and handle empty names
            std::string displayName = (i < names.size() && !names[i].empty()) ? names[i] : "Unknown";
            countryNames[i] = sf::Text(displayName, font);

            // Create a vector of coordinates from the polygon points
            std::vector<sf::Vector2f> coordinates;
            for (size_t k = 0; k < polygon.getVertexCount(); ++k) {
                coordinates.push_back(polygon[k].position);
            }

            // Calculate the centroid of the polygon for positioning
            sf::Vector2f centroid = calculateCentroid(coordinates);

            // Calculate bounding box size of the polygon for scaling the text
            float polygonWidth = maxBounds.x - minBounds.x;
            float polygonHeight = maxBounds.y - minBounds.y;
            float minDimension = std::min(polygonWidth, polygonHeight);

            // Set text origin to its center for accurate centering
            countryNames[i].setOrigin(countryNames[i].getLocalBounds().width / 2.0f, countryNames[i].getLocalBounds().height / 2.0f);
            countryNames[i].setCharacterSize(static_cast<unsigned int>(minDimension / 5) + rendererSettings.fontSize); // Adjust divisor as needed

            countryNames[i].setPosition(
                static_cast<float>((centroid.x - minBounds.x) * (rendererSettings.scale.x + scale) + (rendererSettings.offset.x + offset.x)),
                static_cast<float>((maxBounds.y - centroid.y) * (rendererSettings.scale.y + scale) + (rendererSettings.offset.y + offset.y))
            );
            countryNames[i].setFillColor(sf::Color(rendererSettings.fontColor[0] * 255, rendererSettings.fontColor[1] * 255, rendererSettings.fontColor[2] * 255));
            countryNames[i].setOutlineColor(sf::Color::Black);
            countryNames[i].setOutlineThickness(1.0f);

        }

    }


    // Only draw country names if toggleNames is true
    if (toggleNames) {
        for (const auto& name : countryNames) {
            window.draw(name);
        }
    }

    // Draw the outline
    for (const auto& polygon : polygons) {
        sf::VertexArray outline(sf::LineStrip, polygon.getVertexCount() + 1);
        for (size_t j = 0; j < polygon.getVertexCount(); j++) {
            outline[j].position = sf::Vector2f(
                static_cast<float>((polygon[j].position.x - minBounds.x) * (rendererSettings.scale.x + scale) + (rendererSettings.offset.x + offset.x)),
                static_cast<float>((maxBounds.y - polygon[j].position.y) * (rendererSettings.scale.y + scale) + (rendererSettings.offset.y + offset.y))
            );
            outline[j].color = sf::Color::Black; // Outline color
        }
        outline[polygon.getVertexCount()].position = outline[0].position;
        outline[polygon.getVertexCount()].color = sf::Color::Black;

        window.draw(outline);
    }
}


void MapRenderer::update(const sf::Vector2f& mousePos, sf::RenderWindow& window, MapDrawTexture& mapDrawTexture) {
    // Convert mouse position to world coordinates considering scale and offset
    sf::Vector2f worldPos;
    worldPos.x = (mousePos.x - offset.x) / scale + minBounds.x;
    worldPos.y = maxBounds.y - (mousePos.y - offset.y) / scale;

    // Calculate the scale and offset to fit the map to the screen
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = mapDrawTexture.getTextureSize();

    float scaleX = static_cast<float>(windowSize.x) / (maxBounds.x - minBounds.x);
    float scaleY = static_cast<float>(windowSize.y) / (maxBounds.y - minBounds.y);
    scale = std::min(scaleX, scaleY);

    offset.x = (windowSize.x - (maxBounds.x - minBounds.x) * scale) / 2.0f;
    offset.y = (windowSize.y - (maxBounds.y - minBounds.y) * scale) / 2.0f;

}

bool MapRenderer::isPointInPolygon(const sf::Vector2f& point, const std::vector<sf::Vector2f>& polygon) {
    bool inside = false;
    for (size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
        const auto& pi = polygon[i];
        const auto& pj = polygon[j];
        
        // Ray casting algorithm using world coordinates
        if (((pi.y > point.y) != (pj.y > point.y)) &&
            (point.x < (pj.x - pi.x) * (point.y - pi.y) / (pj.y - pi.y) + pi.x)) {
            inside = !inside;
        }
    }
    return inside;
}

void MapRenderer::updateSelectedColor(const sf::Color& color) {
    if (selectedCountry.empty()) {
        return;
    }
    
    for (size_t i = 0; i < names.size(); i++) {
        if (names[i] == selectedCountry) {
            colors[i] = color;
            break;
        }
    }
}
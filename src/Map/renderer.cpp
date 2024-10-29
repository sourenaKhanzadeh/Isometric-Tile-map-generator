#include "renderer.hpp"

void MapRenderer::calculateBounds() {
    if (polygons.empty()) return;
    
    // Initialize bounds with first coordinate
    minBounds = polygons[0].coordinates[0];
    maxBounds = polygons[0].coordinates[0];
    
    // Single pass bounds calculation
    #pragma omp parallel for reduction(min:minBounds.x,minBounds.y) reduction(max:maxBounds.x,maxBounds.y)
    for (size_t i = 0; i < polygons.size(); i++) {
        for (const auto& coord : polygons[i].coordinates) {
            minBounds.x = std::min(minBounds.x, coord.x);
            maxBounds.x = std::max(maxBounds.x, coord.x);
            minBounds.y = std::min(minBounds.y, coord.y);
            maxBounds.y = std::max(maxBounds.y, coord.y);
        }
    }
}

void MapRenderer::calculateScaleAndOffset(const sf::Vector2u& windowSize) {
    // Cache window dimensions
    const double winWidth = static_cast<double>(windowSize.x);
    const double winHeight = static_cast<double>(windowSize.y);
    
    // Calculate scale once
    const double width = maxBounds.x - minBounds.x;
    const double height = maxBounds.y - minBounds.y;
    scale = std::min(winWidth / width, winHeight / height);

    // Precalculate map dimensions
    const double mapWidth = width * scale;
    const double mapHeight = height * scale;

    // Center map
    offset.x = (winWidth - mapWidth) / 2.0f;
    offset.y = (winHeight - mapHeight) / 2.0f;
}

void MapRenderer::generateColors() {
    colors.resize(polygons.size(), sf::Color(128, 128, 128));
}

MapRenderer::MapRenderer(const std::string& filename) : filename(filename) {
    // Reserve space for large datasets
    polygons.reserve(1000);
    colors.reserve(1000);
}

void MapRenderer::loadFromGeoJSON() {
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

    #pragma omp parallel for
    for (size_t i = 0; i < features.size(); i++) {
        const auto& feature = features[i];
        names.push_back(feature["properties"]["name"].get<std::string>());
        const auto& geometry = feature["geometry"];
        const std::string& geomType = geometry["type"];

        if (geomType == "Polygon") {
            #pragma omp critical
            addPolygon(geometry["coordinates"]);
        } else if (geomType == "MultiPolygon") {
            for (const auto& polygon : geometry["coordinates"]) {
                #pragma omp critical
                addPolygon(polygon);
            }
        }
    }
}

void MapRenderer::addPolygon(const json& coordinates) {
    Polygon polygon;
    const auto& points = coordinates[0];
    polygon.coordinates.reserve(points.size());
    
    for (const auto& point : points) {
        polygon.coordinates.emplace_back(Coordinate{
            point[0].get<double>(),
            point[1].get<double>()
        });
    }
    polygons.push_back(std::move(polygon));
}

void MapRenderer::draw(sf::RenderWindow& window) {
    calculateScaleAndOffset(window.getSize());

    // Create vertex arrays for batch rendering
    std::vector<sf::VertexArray> shapes;
    shapes.reserve(polygons.size());

    #pragma omp parallel for
    for (size_t i = 0; i < polygons.size(); i++) {
        const auto& polygon = polygons[i];
        sf::VertexArray shape(sf::TriangleFan, polygon.coordinates.size());
        
        for (size_t j = 0; j < polygon.coordinates.size(); j++) {
            float x = static_cast<float>((polygon.coordinates[j].x - minBounds.x) * scale + offset.x);
            float y = static_cast<float>((maxBounds.y - polygon.coordinates[j].y) * scale + offset.y);
            shape[j].position = sf::Vector2f(x, y);
            shape[j].color = colors[i];
        }

        #pragma omp critical
        shapes.push_back(std::move(shape));
    }

    // Batch render all shapes
    for (const auto& shape : shapes) {
        window.draw(shape);
    }
}

void MapRenderer::update(const sf::Vector2f& mousePos) {
    // Convert mouse position to world coordinates considering scale and offset
    sf::Vector2f worldPos;
    worldPos.x = (mousePos.x - offset.x) / scale + minBounds.x;
    worldPos.y = maxBounds.y - (mousePos.y - offset.y) / scale;

    // Implement hover effect
    for (size_t i = 0; i < polygons.size(); i++) {
        if (isPointInPolygon(worldPos, polygons[i].coordinates)) {
            // if pressed, select the country
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                selectedCountry = names[i];
                selectedPolygon = polygons[i];
                colors[i] = sf::Color(255, 255, 255);
            }
        }
    }

    // if no country is selected, reset the colors
    for(size_t i = 0; i < polygons.size(); i++){
        if(names[i] != selectedCountry && colors[i] == sf::Color(255, 255, 255)){
            colors[i] = sf::Color(128, 128, 128);
        }
    }

}

bool MapRenderer::isPointInPolygon(const sf::Vector2f& point, const std::vector<Coordinate>& polygon) {
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

MapDrawTexture::MapDrawTexture() {
    lowResTexture.create(1920, 1080);
    highResTexture.create(7680, 4320);
    highResTexture.loadFromFile("res/Earth-Large.png");
    lowResTexture.loadFromFile("res/Earth-Small.png");


    lowResSprite.setTexture(lowResTexture);
    highResSprite.setTexture(highResTexture);

    mapSprite.setTexture(lowResTexture);  // Start with low-res texture
}


void MapDrawTexture::updateMapTexture(float zoomFactor, const sf::Vector2u& windowSize) {
    currentZoomFactor = zoomFactor;

    // Switch to high resolution if zoomed in enough, otherwise use low resolution
    if (zoomFactor < 0.5f && isHighResActive) {  // Far out: low resolution
        mapSprite.setTexture(lowResTexture, true);
        isHighResActive = false;
    } else if (zoomFactor >= 0.5f && !isHighResActive) {  // Close in: switch to high resolution
        mapSprite.setTexture(highResTexture, true);
        isHighResActive = true;
    }

    // Calculate scaling factor to fit the map to the screen for both textures
    sf::Vector2u textureSize = mapSprite.getTexture()->getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    
    // Apply uniform scaling to fit the screen
    mapSprite.setScale(scaleX * zoomFactor, scaleY * zoomFactor);

    // Center the map sprite based on window size and scaled texture dimensions
    mapSprite.setPosition(
        (windowSize.x - textureSize.x * mapSprite.getScale().x) / 2.0f,
        (windowSize.y - textureSize.y * mapSprite.getScale().y) / 2.0f
    );
}

void MapDrawTexture::draw(sf::RenderWindow& window) {
    window.draw(mapSprite);
}
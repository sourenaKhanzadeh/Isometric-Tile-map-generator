#include "contours.hpp"

Contours::Contours(std::string contoursPath) : contoursPath(contoursPath) {
}

void Contours::loadContours() {
    std::ifstream file(contoursPath);
    nlohmann::json contoursJson = nlohmann::json::parse(file);
    contours = contoursJson.get<std::vector<std::vector<int>>>();
}

std::vector<std::vector<int>> Contours::getContours() {
    return contours;
}


void Contours::calculateBounds() {
    // Initialize bounds with extreme values
    minBounds = sf::Vector2f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    maxBounds = sf::Vector2f(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

    for (const auto& contour : contours) {
        for (size_t i = 0; i < contour.size(); i += 2) {
            float x = contour[i];
            float y = contour[i + 1];
            minBounds.x = std::min(minBounds.x, x);
            minBounds.y = std::min(minBounds.y, y);
            maxBounds.x = std::max(maxBounds.x, x);
            maxBounds.y = std::max(maxBounds.y, y);
        }
    }
}


void Contours::draw(sf::RenderWindow& window, float zoomFactor) {
    // Define contour bounds (assuming they were already calculated)
    sf::Vector2u windowSize = window.getSize();
    float contourWidth = maxBounds.x - minBounds.x;
    float contourHeight = maxBounds.y - minBounds.y;

    // Calculate scaling factors
    float scaleX = static_cast<float>(windowSize.x) / contourWidth;
    float scaleY = static_cast<float>(windowSize.y) / contourHeight;
    float scale = std::min(scaleX, scaleY) * zoomFactor;

    // Calculate offset to center the contours
    sf::Vector2f offset(
        (windowSize.x - contourWidth * scale) / 2.0f - minBounds.x * scale,
        (windowSize.y - contourHeight * scale) / 2.0f - minBounds.y * scale
    );

    // Draw contours with applied scale and offset
    for (const auto& contour : contours) {
        sf::VertexArray lineStrip(sf::PrimitiveType::LineStrip, contour.size() / 2);
        for (size_t i = 0; i < contour.size(); i += 2) {
            float x = contour[i] * scale + offset.x;
            float y = contour[i + 1] * scale + offset.y;
            lineStrip[i / 2].position = sf::Vector2f(x, y);
            lineStrip[i / 2].color = contourColor;
        }
        window.draw(lineStrip);
    }

    calculateBounds();
}




void Contours::asyncLoadContours(ProgressBar& progressBar) {
    std::future<void> future = std::async(std::launch::async, [this, &progressBar]() {
        loadContours();
        progressBar.incrementProgress();
    });
    future.get();
}


void Contours::changeColor(sf::Color color) {
    contourColor = color;
}

Contours::~Contours() {
    contours.clear();
}

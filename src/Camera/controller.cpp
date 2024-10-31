#include "controller.hpp"

CameraController::CameraController(sf::View& view, float moveSpeed, float zoomFactor, float minZoom, float maxZoom)
    : view(view), moveSpeed(moveSpeed), zoomFactor(1.0f), minZoom(1.0f), maxZoom(10.0f) {
    // Set the initial view size based on a zoom factor of 1.0
    view.setSize(1920.0f, 1080.0f);
}

void CameraController::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) { // Zoom in
            zoomFactor = std::min(zoomFactor * 1.1f, maxZoom); // Increase zoom factor, clamp to maxZoom
        } else if (event.mouseWheelScroll.delta < 0) { // Zoom out
            zoomFactor = std::max(zoomFactor / 1.1f, minZoom); // Decrease zoom factor, clamp to minZoom
        }

        // Adjust view size based on updated zoom factor
        view.setSize(1920.0f / zoomFactor, 1080.0f / zoomFactor);
    }
}

void CameraController::update() {
    float zoomLevel = view.getSize().x / 1920.0f;

    // Handle panning
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

    const float mapWidth = 1920.0f;
    const float mapHeight = 1080.0f;
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();

    float minX = (1 - zoomFactor) * mapWidth / 2;
    float maxX = mapWidth - minX;
    float minY = (1 - zoomFactor) * mapHeight / 2;
    float maxY = mapHeight - minY;

    float clampedX = std::clamp(viewCenter.x, minX, maxX);
    float clampedY = std::clamp(viewCenter.y, minY, maxY);

    view.setCenter(clampedX, clampedY); // Apply clamped values
}

float CameraController::getZoomFactor() const {
    return zoomFactor;
}

sf::Vector2f CameraController::getOffsetWithZoom() const {
    return view.getCenter() - sf::Vector2f(1920 / 2, 1080 / 2);
}

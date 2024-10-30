#include "controller.hpp"

CameraController::CameraController(sf::View& view, float moveSpeed, float zoomFactor, float minZoom, float maxZoom)
    : view(view), moveSpeed(moveSpeed), zoomFactor(zoomFactor), minZoom(1.0f), maxZoom(10.0f) {
    // Make the map fit the screen initially
    view.setSize(1920, 1080);
    view.setCenter(1920 / 2, 1080 / 2);
    
    currentZoom = 1.0f;
}

void CameraController::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) { // Zoom in
            zoomFactor = std::min(zoomFactor * 1.1f, maxZoom); // Increase zoom factor, clamp to maxZoom
        } else if (event.mouseWheelScroll.delta < 0) { // Zoom out
            zoomFactor = std::max(zoomFactor / 1.1f, minZoom); // Decrease zoom factor, clamp to minZoom
        }

        // Adjust view size based on zoom factor
        view.setSize(1920 / zoomFactor, 1080 / zoomFactor);
        
    }
}


void CameraController::update() {
    // Use zoom level to adjust movement speed for smooth panning
    float zoomLevel = view.getSize().x / 1920.0f;

    // Handle panning movement
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

    // Define the map boundaries (assuming the map is 1920x1080 in size)
    const float mapWidth = 1920.0f;
    const float mapHeight = 1080.0f;

    // Get the current view center and size
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();

    // Calculate dynamic clamping boundaries to allow full exploration
    float minX = viewSize.x / 2.0f;
    float maxX = mapWidth - viewSize.x / 2.0f;
    float minY = viewSize.y / 2.0f;
    float maxY = mapHeight - viewSize.y / 2.0f;

    // Ensure view center is within the calculated boundaries
    float clampedX = std::clamp(viewCenter.x, minX, maxX);
    float clampedY = std::clamp(viewCenter.y, minY, maxY);

    // Update the view center to the clamped values
    // view.setCenter(clampedX, clampedY);
}

float CameraController::getZoomFactor() const {
    return zoomFactor;
}

sf::Vector2f CameraController::getOffsetWithZoom() const {
    return view.getCenter() - sf::Vector2f(1920 / 2, 1080 / 2);
}


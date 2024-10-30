#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include <SFML/Graphics.hpp>

class CameraController {
private:
    sf::View& view;
    float moveSpeed;
    float zoomFactor;
    float minZoom;
    float maxZoom;

    float currentZoom;

public:
    CameraController(sf::View& view, float moveSpeed = 10.0f, float zoomFactor = 1.1f, float minZoom = 0.5f, float maxZoom = 10.0f);
    void handleEvent(const sf::Event& event);
    void update();
    float getZoomFactor() const;
    sf::Vector2f getOffsetWithZoom() const;
};

#endif // CAMERA_CONTROLLER_HPP

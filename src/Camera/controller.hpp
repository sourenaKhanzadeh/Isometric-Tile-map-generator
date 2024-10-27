#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include <SFML/Graphics.hpp>

class CameraController {
private:
    sf::View& view;
    float moveSpeed;
    float zoomFactor;

public:
    CameraController(sf::View& view, float moveSpeed = 10.0f, float zoomFactor = 1.1f);
    void handleEvent(const sf::Event& event);
    void update();
};

#endif // CAMERA_CONTROLLER_HPP

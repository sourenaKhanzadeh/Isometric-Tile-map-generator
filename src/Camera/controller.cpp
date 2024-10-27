#include "controller.hpp"


CameraController::CameraController(sf::View& view, float moveSpeed, float zoomFactor)
    : view(view), moveSpeed(moveSpeed), zoomFactor(zoomFactor) {}

void CameraController::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
            view.zoom(1.0f / zoomFactor);
        } else if (event.mouseWheelScroll.delta < 0) {
            view.zoom(zoomFactor);
        }
    }
}

void CameraController::update() {
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
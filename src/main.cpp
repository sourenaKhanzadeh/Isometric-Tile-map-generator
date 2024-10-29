#include <SFML/Graphics.hpp>
#include <SFML/System.hpp> // For SFML threading
#include <iostream>
#include <memory>
#include "Camera/controller.hpp"
#include "Map/renderer.hpp"
#include "Utils/color_swatches.hpp"


int main() {
    // Main game window setup
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Fortifier: Forge and Conquer");
    window.setFramerateLimit(144);

    sf::View view(sf::FloatRect(0, 0, 1920, 1080));

    // MapRenderer mapRenderer("./countries.geo.json");
    MapRenderer mapRenderer("./countries.geo.json");
    mapRenderer.loadFromGeoJSON();
    mapRenderer.calculateBounds();
    mapRenderer.generateColors();
    MapDrawTexture mapDrawTexture(mapRenderer);

    CameraController cameraController(view);

    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }


            cameraController.handleEvent(event);
        }

        window.setView(view);
        cameraController.update();
        mapRenderer.update(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
        // Render main game window
        window.clear(sf::Color::Black);
        mapRenderer.draw(window);
        mapDrawTexture.draw(window);
        window.display();
    }


    return 0;
}

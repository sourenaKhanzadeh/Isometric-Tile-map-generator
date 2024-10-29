#include <SFML/Graphics.hpp>
#include <SFML/System.hpp> // For SFML threading
#include <iostream>
#include <memory>
#include <thread>
#include "Camera/controller.hpp"
#include "Map/renderer.hpp"
#include "Utils/progressbar.hpp"
#include <imgui.h>
#include <imgui-sfml.h>

#define DEBUG 1
int main() {
    // Main game window setup
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Fortifier: Forge and Conquer");
    window.setFramerateLimit(144);

#if DEBUG == 0
    // anchor the progress bar to bottom center of the screen
    ProgressBar progressBar(window, sf::Vector2f(1920, 20), sf::Vector2f(0, 1080 - 20), sf::Color::Green, sf::Color::White);
    progressBar.setTotalItems(2);

    sf::View view(sf::FloatRect(0, 0, 1920, 1080));

    // MapRenderer mapRenderer("./countries.geo.json");
    MapRenderer mapRenderer("./countries.geo.json");
    mapRenderer.loadFromGeoJSON();
    mapRenderer.calculateBounds();
    mapRenderer.generateColors();
    MapDrawTexture mapDrawTexture(progressBar);

    CameraController cameraController(view);

    std::thread textureLoadingThread([&mapDrawTexture]() {
        mapDrawTexture.loadTexturesAsync();
    });


    progressBar.update("Loading textures...");

    textureLoadingThread.join();
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
        mapDrawTexture.updateMapTexture(cameraController.getZoomFactor(), window.getSize());
        window.display();
    }

#else
    // Debug mode
#endif

    return 0;
}

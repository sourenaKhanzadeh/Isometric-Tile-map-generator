#include "Camera/controller.hpp"
#include "Map/renderer.hpp"
#include "Utils/color_swatches.hpp"


int main() {
    sf::RenderWindow window({1920u, 1080u}, "Fortifier: Forge and Conquer");
    window.setFramerateLimit(144);

    sf::View view(sf::FloatRect(0, 0, 1920, 1080));
    window.setView(view);

    MapRenderer mapRenderer("./countries.geo.json");
    CameraController cameraController(view);
    sf::RenderWindow colorSwatchesWindow({400u, 400u}, "Color Swatches");
    Utils::ColorPicker colorSwatches(colorSwatchesWindow);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            cameraController.handleEvent(event);
        }

        while (colorSwatchesWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                colorSwatchesWindow.close();
            colorSwatches.handleEvent(event);
        }

        cameraController.update();
        window.setView(view);

        window.clear();
        colorSwatchesWindow.clear();
        mapRenderer.draw(window);
        colorSwatches.draw();

        window.display();
        colorSwatchesWindow.display();
    }

    return 0;
}

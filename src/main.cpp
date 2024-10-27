#include "Camera/controller.hpp"
#include "Map/renderer.hpp"


int main() {
    sf::RenderWindow window({1920u, 1080u}, "Fortifier: Forge and Conquer");
    window.setFramerateLimit(144);

    sf::View view(sf::FloatRect(0, 0, 1920, 1080));
    window.setView(view);

    MapRenderer mapRenderer("./countries.geo.json");
    CameraController cameraController(view);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            cameraController.handleEvent(event);
        }

        cameraController.update();
        window.setView(view);

        window.clear();
        mapRenderer.draw(window);
        window.display();
    }

    return 0;
}

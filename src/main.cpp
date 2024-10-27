#include <SFML/Graphics.hpp>
#include <SFML/System.hpp> // For SFML threading
#include <iostream>
#include <memory>
#include "Camera/controller.hpp"
#include "Map/renderer.hpp"
#include "Utils/color_swatches.hpp"

// Global flag to indicate if color swatch window is active
bool colorPickerActive = false;
sf::Mutex colorPickerMutex;  // SFML mutex to synchronize access to colorPickerActive

// Function to run the color swatch window on a separate SFML thread
void runColorSwatchWindow() {
    sf::RenderWindow colorSwatchesWindow(sf::VideoMode(400, 400), "Color Swatches");
    Utils::ColorPicker colorSwatches(colorSwatchesWindow);

    while (colorSwatchesWindow.isOpen()) {
        sf::Event colorSwatchEvent;
        while (colorSwatchesWindow.pollEvent(colorSwatchEvent)) {
            if (colorSwatchEvent.type == sf::Event::Closed) {
                colorSwatchesWindow.close();
                sf::Lock lock(colorPickerMutex); // Lock to safely modify colorPickerActive
                colorPickerActive = false;
            } else {
                colorSwatches.handleEvent(colorSwatchEvent);
            }
        }

        colorSwatchesWindow.clear(sf::Color::Black);
        colorSwatches.draw();
        colorSwatchesWindow.display();
    }
}

// Function to toggle color swatch window
void toggleColorSwatchWindow(std::unique_ptr<sf::Thread>& colorSwatchThread) {
    sf::Lock lock(colorPickerMutex); // Lock to safely access colorPickerActive

    if (colorPickerActive) {
        // Close the color swatch window if it's active
        colorPickerActive = false;
        if (colorSwatchThread) {
            colorSwatchThread->wait();  // Just call wait() without checking its return value
            colorSwatchThread.reset();
        }
    } else {
        // Start a new thread to open the color swatch window
        colorPickerActive = true;
        colorSwatchThread = std::make_unique<sf::Thread>(&runColorSwatchWindow);
        colorSwatchThread->launch();
    }
}

int main() {
    // Main game window setup
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Fortifier: Forge and Conquer");
    window.setFramerateLimit(144);

    sf::View view(sf::FloatRect(0, 0, 1920, 1080));
    window.setView(view);

    MapRenderer mapRenderer("./countries.geo.json");
    mapRenderer.loadFromGeoJSON();
    mapRenderer.calculateBounds();
    mapRenderer.generateColors();

    CameraController cameraController(view);

    std::unique_ptr<sf::Thread> colorSwatchThread;

    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                sf::Lock lock(colorPickerMutex); // Lock to safely modify colorPickerActive
                colorPickerActive = false;

                // Wait for color swatch thread to finish if active
                if (colorSwatchThread) {
                    colorSwatchThread->wait();  // Just call wait() without checking its return value
                    colorSwatchThread.reset();
                }
            }

            // Toggle color swatch window with F1 key
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1) {
                toggleColorSwatchWindow(colorSwatchThread);
            }

            cameraController.handleEvent(event);
        }

        // Render main game window
        window.clear(sf::Color::Black);
        mapRenderer.draw(window);
        window.display();
    }

    // Wait for the color swatch thread to finish when the main window closes
    if (colorSwatchThread) {
        colorSwatchThread->wait();  // Just call wait() without checking its return value
        colorSwatchThread.reset();
    }

    return 0;
}

#include <SFML/Graphics.hpp>
#include <thread>
#include <future>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include "Camera/controller.hpp"
#include "Map/renderer.hpp"
#include "Utils/color_swatches.hpp"

// Flags and synchronization tools
std::atomic<bool> isRunning(true);
std::atomic<bool> colorPickerActive(false);
std::condition_variable colorSwatchCondition;
std::mutex colorSwatchMutex;

// Async resource loading function
std::future<void> loadResourcesAsync(MapRenderer& mapRenderer) {
    return std::async(std::launch::async, [&mapRenderer] {
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate loading delay
        mapRenderer.loadFromGeoJSON();
        mapRenderer.calculateBounds();
        mapRenderer.generateColors();
        std::cout << "Resources loaded asynchronously.\n";
    });
}

// Placeholder function for future game update logic
void placeholderGameUpdate(float deltaTime) {
    // Future game logic (physics, events) would go here
}

// Thread function to run the color swatch window
void runColorSwatchWindow() {
    while (isRunning) {
        // Wait until colorPickerActive is true
        {
            std::unique_lock<std::mutex> lock(colorSwatchMutex);
            colorSwatchCondition.wait(lock, [] { return colorPickerActive || !isRunning; });
            if (!isRunning) return;  // Exit if the game is closing
        }

        // Create the color swatch window
        sf::RenderWindow colorSwatchesWindow(sf::VideoMode(400, 400), "Color Swatches");
        Utils::ColorPicker colorSwatches(colorSwatchesWindow);

        // Handle events and rendering for color swatch window
        while (colorSwatchesWindow.isOpen() && colorPickerActive) {
            sf::Event colorSwatchEvent;
            while (colorSwatchesWindow.pollEvent(colorSwatchEvent)) {
                if (colorSwatchEvent.type == sf::Event::Closed) {
                    colorPickerActive = false;
                } else {
                    colorSwatches.handleEvent(colorSwatchEvent);
                }
            }
            colorSwatchesWindow.clear(sf::Color::Black);
            colorSwatches.draw();
            colorSwatchesWindow.display();
        }

        // Close the window after exiting loop
        colorSwatchesWindow.close();

        // Notify that the window has been closed
        {
            std::lock_guard<std::mutex> lock(colorSwatchMutex);
            colorPickerActive = false;
        }
    }
}

// Main function to toggle color swatch and handle game loop
int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Fortifier: Forge and Conquer");
    window.setFramerateLimit(144);

    sf::View view(sf::FloatRect(0, 0, 1920, 1080));
    window.setView(view);

    MapRenderer mapRenderer("./countries.geo.json");
    CameraController cameraController(view);

    // Asynchronously load resources
    auto resourceLoader = loadResourcesAsync(mapRenderer);

    // Set up timing variables for fixed timestep updates
    const float fixedTimeStep = 1.0f / 60.0f;
    float accumulatedTime = 0.0f;
    sf::Clock clock;

    // Start color swatch thread (but it waits until signaled to open the window)
    std::thread colorSwatchThread(runColorSwatchWindow);

    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                isRunning = false;
                colorSwatchCondition.notify_all();
                colorSwatchThread.join();
            }

            // Toggle color swatch window with F1 key
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1) {
                std::lock_guard<std::mutex> lock(colorSwatchMutex);
                colorPickerActive = !colorPickerActive;
                colorSwatchCondition.notify_all();  // Signal thread to open or close window
            }

            cameraController.handleEvent(event);
        }

        // Calculate elapsed time
        float deltaTime = clock.restart().asSeconds();
        accumulatedTime += deltaTime;

        // Ensure resources are loaded before proceeding with updates
        if (resourceLoader.valid() && resourceLoader.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            resourceLoader.get();  // Complete resource loading
        }

        // Fixed timestep updates
        while (accumulatedTime >= fixedTimeStep) {
            placeholderGameUpdate(fixedTimeStep);
            accumulatedTime -= fixedTimeStep;
        }

        // Update camera
        cameraController.update();
        window.setView(view);

        // Render main window
        window.clear(sf::Color::Black);
        mapRenderer.draw(window);
        window.display();
    }

    // Ensure color swatch thread exits when main window closes
    if (colorSwatchThread.joinable()) {
        isRunning = false;
        colorSwatchCondition.notify_all();
        colorSwatchThread.join();
    }

    return 0;
}

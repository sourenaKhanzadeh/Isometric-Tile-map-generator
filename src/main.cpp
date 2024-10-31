#include <SFML/Graphics.hpp>
#include <SFML/System.hpp> // For SFML threading
#include <iostream>
#include <memory>
#include <thread>
#include "Camera/controller.hpp"
#include "Map/contours.hpp"
#include "Map/renderer.hpp"
#include "Utils/progressbar.hpp"
#include <imgui.h>
#include <imgui-sfml.h>
#include "Map/plot.hpp"
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
    if (!ImGui::SFML::Init(window)) {
        std::cerr << "Failed to initialize ImGui-SFML" << std::endl;
        return -1;
    }
    // anchor the progress bar to bottom center of the screen
    ProgressBar progressBar(window, sf::Vector2f(1920, 20), sf::Vector2f(0, 1080 - 20), sf::Color::Green, sf::Color::White);
    progressBar.setTotalItems(3);

    sf::View view(sf::FloatRect(0, 0, 1920, 1080));
     // MapRenderer mapRenderer("./countries.geo.json");

    MapDrawTexture mapDrawTexture(progressBar);

    CameraController cameraController(view);

    std::thread textureLoadingThread([&mapDrawTexture]() {
        mapDrawTexture.loadTexturesAsync();
    });
    MapRenderer mapRenderer("./countries.geo.json", progressBar);
    mapRenderer.loadFromGeoJSON();
    mapRenderer.calculateBounds();

    progressBar.update("Loading textures...");

    textureLoadingThread.join();
    sf::Clock deltaClock;
    float mapColors[3] = {0.f, 0.f, 0.f};
    float contourColor[3] = {0.f, 0.f, 0.f};
    std::string countryName(100, '\0');
    RendererSettings rendererSettings = {sf::Vector2f(0.0, 0.91), sf::Vector2f(0.0, -70.0)};
    PlotOnMap *plotOnMap = new PlotOnMap();
    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed) {
                window.close();
                ImGui::SFML::Shutdown(window);
            }
            cameraController.handleEvent(event);
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Debug");
        ImGui::Text("FPS: %lf", ImGui::GetIO().Framerate);
        ImGui::Text("Zoom: %lf", cameraController.getZoomFactor());
        ImGui::Text("Mouse Position: %lf, %lf", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
        ImGui::ColorEdit3("Color", mapColors);
        ImGui::ColorEdit3("Contour Color", contourColor);
        if(ImGui::CollapsingHeader("Renderer: Settings")){
            if(ImGui::CollapsingHeader("Names")){
                ImGui::Checkbox("Toggle Names", &mapRenderer.toggleNames);
                ImGui::SliderFloat("Font Size", &rendererSettings.fontSize, 1.0, 100.0);
                ImGui::ColorEdit3("Font Color", rendererSettings.fontColor);
            }
            if(ImGui::CollapsingHeader("Scale and Offset")){
                ImGui::SliderFloat("ScaleX", &rendererSettings.scale.x, 0.0, 1.0);
                ImGui::SliderFloat("ScaleY", &rendererSettings.scale.y, 0.0, 1.0);
                ImGui::SliderFloat("OffsetX", &rendererSettings.offset.x, -1000.0, 1000.0);
                ImGui::SliderFloat("OffsetY", &rendererSettings.offset.y, -1000.0, 1000.0);
            }
        }
        ImGui::End();
        // Obtain map scaling and offset
        sf::Vector2u windowSize = window.getSize();
        sf::Vector2u textureSize = mapDrawTexture.getTextureSize();

        sf::Color updatedContourColor(static_cast<sf::Uint8>(contourColor[0] * 255),
                                  static_cast<sf::Uint8>(contourColor[1] * 255),
                                      static_cast<sf::Uint8>(contourColor[2] * 255));
        float mapScale = cameraController.getZoomFactor();
        sf::Vector2f mapOffset = cameraController.getOffsetWithZoom();
        window.setView(view);
        cameraController.update();
        mapRenderer.update(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
        // Render main game window
        window.clear(sf::Color::Black);
        // mapRenderer.draw(window);
        mapDrawTexture.draw(window);
        mapDrawTexture.updateMapTexture(cameraController.getZoomFactor(), window.getSize());
        plotOnMap->draw(window, cameraController.getZoomFactor());
        mapRenderer.draw(window, cameraController.getZoomFactor(), rendererSettings, textureSize);
        ImGui::SFML::Render(window);
        window.display();
    }
    delete plotOnMap;
    ImGui::SFML::Shutdown(window);

#endif

    return 0;
}

#include "map_texture.hpp"

MapDrawTexture::MapDrawTexture(ProgressBar& progressBar)
    : progressBar(progressBar) {}

void MapDrawTexture::loadTexturesAsync() {
    // Run each texture loading in a separate async task
    std::vector<std::future<void>> futures;

    futures.emplace_back(std::async(std::launch::async, [this]() {
        lowResTexture.loadFromFile("res/Earth-Small.png");
        progressBar.incrementProgress();
    }));

    futures.emplace_back(std::async(std::launch::async, [this]() {
        highResTexture.loadFromFile("res/Earth-Large.png");
        progressBar.incrementProgress();
    }));

    // Wait for all textures to load
    for (auto& future : futures) {
        future.get(); // Wait for each async task to complete
    }

    // Set the initial texture after loading
    mapSprite.setTexture(lowResTexture);
}

void MapDrawTexture::updateMapTexture(float zoomFactor, const sf::Vector2u& windowSize) {
    currentZoomFactor = zoomFactor;

    // Switch to high resolution if zoomed in enough, otherwise use low resolution
    if (zoomFactor < 0.5f && isHighResActive) {  // Far out: low resolution
        mapSprite.setTexture(lowResTexture, true);
        isHighResActive = false;
    } else if (zoomFactor >= 0.5f && !isHighResActive) {  // Close in: switch to high resolution
        mapSprite.setTexture(highResTexture, true);
        isHighResActive = true;
    }

    // Calculate scaling factor to fit the map to the screen for both textures
    sf::Vector2u textureSize = mapSprite.getTexture()->getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;

    // Apply uniform scaling to fit the screen
    mapSprite.setScale(scaleX * zoomFactor, scaleY * zoomFactor);

    // Center the map sprite based on window size and scaled texture dimensions
    mapSprite.setPosition(
        (windowSize.x - textureSize.x * mapSprite.getScale().x) / 2.0f,
        (windowSize.y - textureSize.y * mapSprite.getScale().y) / 2.0f
    );
}

void MapDrawTexture::draw(sf::RenderWindow& window) {
    window.draw(mapSprite);
}

sf::Vector2u MapDrawTexture::getTextureSize() {
    return lowResTexture.getSize();
}


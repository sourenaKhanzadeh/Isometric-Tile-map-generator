#ifndef MAP_TEXTURE_HPP
#define MAP_TEXTURE_HPP

#include <SFML/Graphics.hpp>
#include "../Utils/progressbar.hpp"
#include <future>

class MapDrawTexture {
public:
    MapDrawTexture(ProgressBar& progressBar);
    void loadTexturesAsync();
    void updateMapTexture(float zoomFactor, const sf::Vector2u& windowSize);
    void draw(sf::RenderWindow& window);
    sf::Vector2u getTextureSize();

private:
    sf::Texture lowResTexture;
    sf::Texture highResTexture;
    sf::Sprite mapSprite;
    bool isHighResActive = false;
    float currentZoomFactor = 1.0f;

    ProgressBar& progressBar;
};

#endif
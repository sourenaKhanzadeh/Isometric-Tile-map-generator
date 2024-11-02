#ifndef GEN_HPP
#define GEN_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <PerlinNoise.hpp>
#include <iostream>
#include "../Camera/controller.hpp"

struct LandmassSettings {
    float octaveMultiplierX = 0.01;
    float octaveMultiplierY = 0.01;
    int octaves = 4;
    int seedValue = 42;
    float waterThreshold = 0.1;
    float plainsThreshold = 0.3;
    float hillsThreshold = 0.6;
};

class LandmassGenerator {
public:
    void generateLandmass();
    LandmassGenerator(LandmassSettings settings);
    void draw(sf::RenderWindow& window);
    LandmassSettings settings;
private:
    const int SCALE = 20;
    const int GRID_WIDTH = 1920 / SCALE;
    const int GRID_HEIGHT = 1080 / SCALE;
    std::vector<std::vector<double>> grid;
    LandmassSettings previousSettings;
};

#endif

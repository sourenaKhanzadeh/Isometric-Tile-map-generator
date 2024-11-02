#ifndef GEN_HPP
#define GEN_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <PerlinNoise.hpp>
#include <iostream>
#include "../Camera/controller.hpp"

struct LandmassSettings {
    float octaveMultiplierX = 0.06;
    float octaveMultiplierY = 0.06;
    int octaves = 20;
    int seedValue = 97088;
    float waterThreshold = 0.49;
    float plainsThreshold = 0.58;
    float hillsThreshold = 1.0;
    float cubeHeightMultiplier = 22.33;
    bool drawGrid = false;
    bool drawCubes = true;
};

class LandmassGenerator {
public:
    void generateLandmass();
    LandmassGenerator(LandmassSettings settings);
    void draw(sf::RenderWindow& window);
    LandmassSettings settings;
private:
    const int SCALE = 5;
    const int GRID_WIDTH = 1920 / SCALE;
    const int GRID_HEIGHT = 1080 / SCALE;
    std::vector<std::vector<double>> grid;
    LandmassSettings previousSettings;
    sf::VertexArray vertexArray;
    std::vector<std::vector<sf::Color>> cachedColors;
    void drawGrid(sf::RenderWindow& window);
    void rebuildVertexArray();
    void makeTile(int x, int y, sf::RenderWindow& window);
    void addCubeVertices(int x, int y);
    void addTileVertices(int x, int y);
    void cacheColors();
};

#endif

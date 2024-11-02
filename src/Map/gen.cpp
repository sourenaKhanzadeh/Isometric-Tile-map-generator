#include "gen.hpp"


void LandmassGenerator::generateLandmass() {
    grid.clear(); // Clear existing data before generating a new landmass

    const siv::PerlinNoise::seed_type seed = settings.seedValue;
    siv::PerlinNoise perlin(seed);

    for (int x = 0; x < GRID_WIDTH; ++x) {
        std::vector<double> row;
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            const double noise = perlin.octave2D_01(
                x * settings.octaveMultiplierX, 
                y * settings.octaveMultiplierY, 
                settings.octaves
            );
            row.push_back(noise);
        }
        grid.push_back(row);
    }

    previousSettings = settings; // Update previous settings
    cacheColors();
    rebuildVertexArray();
    
}

LandmassGenerator::LandmassGenerator(LandmassSettings settings) : settings(settings), previousSettings(settings) {
    generateLandmass();
}


void LandmassGenerator::rebuildVertexArray() {
    vertexArray.clear();
    vertexArray.setPrimitiveType(sf::Quads);

    if (settings.drawCubes) {
        vertexArray.resize(GRID_WIDTH * GRID_HEIGHT * 12); // Allocate space for cubes
        for (int x = 0; x < GRID_WIDTH; ++x) {
            for (int y = 0; y < GRID_HEIGHT; ++y) {
                addCubeVertices(x, y);
            }
        }
    } else {
        vertexArray.resize(GRID_WIDTH * GRID_HEIGHT * 4); // Allocate space for flat tiles
        for (int x = 0; x < GRID_WIDTH; ++x) {
            for (int y = 0; y < GRID_HEIGHT; ++y) {
                addTileVertices(x, y);
            }
        }
    }
}


void LandmassGenerator::addCubeVertices(int x, int y) {
    // Get noise-based elevation and position in isometric view
    double noiseValue = grid[x][y];
    float cubeHeight = noiseValue * settings.cubeHeightMultiplier;
    float isoX = (x - y) * (SCALE / 2);
    float isoY = (x + y) * (SCALE / 4);

    // Define colors based on terrain type
    sf::Color topColor, sideColor;
    if (noiseValue < settings.waterThreshold) {
        topColor = sf::Color(0, 105, 148);  // Ocean Blue
        sideColor = sf::Color(0, 75, 105);  // Shadowed side
    } else if (noiseValue < settings.plainsThreshold) {
        topColor = sf::Color(34, 139, 34);  // Forest Green
        sideColor = sf::Color(24, 100, 24);
    } else if (noiseValue < settings.hillsThreshold) {
        topColor = sf::Color(205, 133, 63); // Brown
        sideColor = sf::Color(139, 69, 19);
    } else {
        topColor = sf::Color(220, 220, 220); // Snow
        sideColor = sf::Color(169, 169, 169);
    }

    // Define top face vertices
    sf::Vector2f topLeft(isoX, isoY - cubeHeight);
    sf::Vector2f topRight(isoX + SCALE / 2, isoY - SCALE / 4 - cubeHeight);
    sf::Vector2f bottomLeft(isoX - SCALE / 2, isoY - SCALE / 4 - cubeHeight);
    sf::Vector2f bottomRight(isoX, isoY - SCALE / 2 - cubeHeight);

    // Add top face vertices
    vertexArray.append(sf::Vertex(topLeft, topColor));
    vertexArray.append(sf::Vertex(topRight, topColor));
    vertexArray.append(sf::Vertex(bottomRight, topColor));
    vertexArray.append(sf::Vertex(bottomLeft, topColor));

    // Left face
    vertexArray.append(sf::Vertex(bottomLeft, sideColor));
    vertexArray.append(sf::Vertex(sf::Vector2f(bottomLeft.x, bottomLeft.y + cubeHeight), sideColor));
    vertexArray.append(sf::Vertex(sf::Vector2f(topLeft.x, topLeft.y + cubeHeight), sideColor));
    vertexArray.append(sf::Vertex(topLeft, sideColor));

    // Right face
    vertexArray.append(sf::Vertex(bottomRight, sideColor));
    vertexArray.append(sf::Vertex(sf::Vector2f(bottomRight.x, bottomRight.y + cubeHeight), sideColor));
    vertexArray.append(sf::Vertex(sf::Vector2f(topRight.x, topRight.y + cubeHeight), sideColor));
    vertexArray.append(sf::Vertex(topRight, sideColor));
}

void LandmassGenerator::addTileVertices(int x, int y) {
    sf::Color tileColor = cachedColors[x][y];
    float posX = x * SCALE;
    float posY = y * SCALE;

    vertexArray.append(sf::Vertex(sf::Vector2f(posX, posY), tileColor));
    vertexArray.append(sf::Vertex(sf::Vector2f(posX + SCALE, posY), tileColor));
    vertexArray.append(sf::Vertex(sf::Vector2f(posX + SCALE, posY + SCALE), tileColor));
    vertexArray.append(sf::Vertex(sf::Vector2f(posX, posY + SCALE), tileColor));
}

void LandmassGenerator::draw(sf::RenderWindow& window) {
    if (settings.cubeHeightMultiplier != previousSettings.cubeHeightMultiplier 
        || settings.drawCubes != previousSettings.drawCubes
        || settings.octaveMultiplierX != previousSettings.octaveMultiplierX
        || settings.octaveMultiplierY != previousSettings.octaveMultiplierY
        || settings.octaves != previousSettings.octaves
        || settings.seedValue != previousSettings.seedValue
        || settings.waterThreshold != previousSettings.waterThreshold
        || settings.plainsThreshold != previousSettings.plainsThreshold
        || settings.hillsThreshold != previousSettings.hillsThreshold
    ) {
        generateLandmass();
        previousSettings = settings; // Update previousSettings here
    }

    window.draw(vertexArray);

    if (settings.drawGrid) {
        drawGrid(window);
    }
}


void LandmassGenerator::cacheColors() {
    cachedColors.clear();
    for (int x = 0; x < GRID_WIDTH; ++x) {
        std::vector<sf::Color> row;
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            double noiseValue = grid[x][y];
            sf::Color tileColor;

            if (noiseValue < settings.waterThreshold) {
                tileColor = sf::Color(0, 105, 148);  // Ocean Blue
            } else if (noiseValue < settings.plainsThreshold) {
                tileColor = sf::Color(34, 139, 34);  // Forest Green
            } else if (noiseValue < settings.hillsThreshold) {
                tileColor = sf::Color(205, 133, 63); // Earthy Brown
            } else {
                tileColor = sf::Color(220, 220, 220); // Snowy White
            }

            // Apply brightness adjustment based on noise value
            int brightnessAdjustment = static_cast<int>(noiseValue * 50);
            tileColor.r = std::min(tileColor.r + brightnessAdjustment, 255);
            tileColor.g = std::min(tileColor.g + brightnessAdjustment, 255);
            tileColor.b = std::min(tileColor.b + brightnessAdjustment, 255);

            row.push_back(tileColor);
        }
        cachedColors.push_back(row);
    }
}


void LandmassGenerator::drawGrid(sf::RenderWindow& window) {
    // Use vertex array for grid to reduce individual draw calls
    sf::VertexArray gridLines(sf::Lines);

    // Vertical lines
    for (int x = 0; x <= GRID_WIDTH; ++x) {
        gridLines.append(sf::Vertex(sf::Vector2f(x * SCALE, 0), sf::Color(50, 50, 50, 100)));
        gridLines.append(sf::Vertex(sf::Vector2f(x * SCALE, GRID_HEIGHT * SCALE), sf::Color(50, 50, 50, 100)));
    }

    // Horizontal lines
    for (int y = 0; y <= GRID_HEIGHT; ++y) {
        gridLines.append(sf::Vertex(sf::Vector2f(0, y * SCALE), sf::Color(50, 50, 50, 100)));
        gridLines.append(sf::Vertex(sf::Vector2f(GRID_WIDTH * SCALE, y * SCALE), sf::Color(50, 50, 50, 100)));
    }

    window.draw(gridLines);
}
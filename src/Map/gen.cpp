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
}

LandmassGenerator::LandmassGenerator(LandmassSettings settings) : settings(settings) {
    generateLandmass();
}

void LandmassGenerator::draw(sf::RenderWindow& window) {
    // Check for changes in settings
    if (settings.seedValue != previousSettings.seedValue ||
        settings.octaveMultiplierX != previousSettings.octaveMultiplierX ||
        settings.octaveMultiplierY != previousSettings.octaveMultiplierY ||
        settings.octaves != previousSettings.octaves ||
        settings.waterThreshold != previousSettings.waterThreshold ||
        settings.plainsThreshold != previousSettings.plainsThreshold ||
        settings.hillsThreshold != previousSettings.hillsThreshold) 
    {
        generateLandmass(); // Regenerate grid if settings have changed
    }

    // Render the grid
    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            sf::RectangleShape rect(sf::Vector2f(SCALE, SCALE));
            rect.setPosition(x * SCALE, y * SCALE);

            // Set color based on elevation thresholds
            double noiseValue = grid[x][y];
            if (noiseValue < settings.waterThreshold) {
                rect.setFillColor(sf::Color(0, 0, 139, 150)); // Water
            } else if (noiseValue < settings.plainsThreshold) {
                rect.setFillColor(sf::Color(34, 139, 34, 180)); // Plains
            } else if (noiseValue < settings.hillsThreshold) {
                rect.setFillColor(sf::Color(139, 69, 19, 200)); // Hills
            } else {
                rect.setFillColor(sf::Color(255, 255, 255, 220)); // Mountains
            }

            makeCube(x, y, window);
        }
    }
}

void LandmassGenerator::makeCube(int x, int y, sf::RenderWindow& window) {
    // Get noise-based elevation value
    double noiseValue = grid[x][y];
    
    // Calculate isometric position and height based on noise
    float isoX = (x - y) * (SCALE / 2);
    float isoY = (x + y) * (SCALE / 4);
    float cubeHeight = noiseValue * 50; // Scale height for different elevation

    // Define the colors for the top and sides based on elevation
    sf::Color topColor, sideColor;
    if (noiseValue < settings.waterThreshold) {
        topColor = sf::Color(0, 0, 139); // Water color
        sideColor = sf::Color(0, 0, 100); // Darker side color for depth
    } else if (noiseValue < settings.plainsThreshold) {
        topColor = sf::Color(34, 139, 34); // Plains color
        sideColor = sf::Color(28, 100, 28); // Darker side color
    } else if (noiseValue < settings.hillsThreshold) {
        topColor = sf::Color(139, 69, 19); // Hills color
        sideColor = sf::Color(100, 50, 15); // Darker side color
    } else {
        topColor = sf::Color(255, 255, 255); // Mountain color
        sideColor = sf::Color(200, 200, 200); // Darker side color
    }

    // Define vertices for the top face
    sf::Vector2f topLeft(isoX, isoY - cubeHeight);
    sf::Vector2f topRight(isoX + SCALE / 2, isoY - SCALE / 4 - cubeHeight);
    sf::Vector2f bottomLeft(isoX - SCALE / 2, isoY - SCALE / 4 - cubeHeight);
    sf::Vector2f bottomRight(isoX, isoY - SCALE / 2 - cubeHeight);

    // Define vertices for the left and right side faces
    sf::VertexArray topFace(sf::Quads, 4);
    topFace[0].position = topLeft;
    topFace[1].position = topRight;
    topFace[2].position = bottomRight;
    topFace[3].position = bottomLeft;
    for (int i = 0; i < 4; i++) topFace[i].color = topColor;

    sf::VertexArray leftFace(sf::Quads, 4);
    leftFace[0].position = bottomLeft;
    leftFace[1].position = sf::Vector2f(bottomLeft.x, bottomLeft.y + cubeHeight);
    leftFace[2].position = sf::Vector2f(topLeft.x, topLeft.y + cubeHeight);
    leftFace[3].position = topLeft;
    for (int i = 0; i < 4; i++) leftFace[i].color = sideColor;

    sf::VertexArray rightFace(sf::Quads, 4);
    rightFace[0].position = bottomRight;
    rightFace[1].position = sf::Vector2f(bottomRight.x, bottomRight.y + cubeHeight);
    rightFace[2].position = sf::Vector2f(topRight.x, topRight.y + cubeHeight);
    rightFace[3].position = topRight;
    for (int i = 0; i < 4; i++) rightFace[i].color = sideColor;

    // Draw each face
    window.draw(leftFace);
    window.draw(rightFace);
    window.draw(topFace);
}



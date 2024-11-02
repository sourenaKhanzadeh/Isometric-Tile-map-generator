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

            window.draw(rect);
        }
    }
}


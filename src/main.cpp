#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

using json = nlohmann::json;

struct LineString {
    std::vector<sf::Vector2f> points;
};

// Function to load JSON data and parse LineString coordinates
std::vector<LineString> loadMapData(const std::string &filename, float &minX, float &maxX, float &minY, float &maxY) {
    std::vector<LineString> lineStrings;

    // Load JSON file
    std::ifstream file(filename);
    json data;
    file >> data;

    minX = minY = std::numeric_limits<float>::max();
    maxX = maxY = std::numeric_limits<float>::min();

    // Parse each LineString in the JSON
    for (const auto &feature : data) {
        if (feature["geometry"]["type"] == "LineString") {
            LineString line;
            for (const auto &coord : feature["geometry"]["coordinates"]) {
                float x = coord[0];
                float y = coord[1];
                line.points.emplace_back(sf::Vector2f(x, y));

                // Track min and max coordinates for scaling
                minX = std::min(minX, x);
                maxX = std::max(maxX, x);
                minY = std::min(minY, y);
                maxY = std::max(maxY, y);
            }
            lineStrings.push_back(line);
        }
    }
    return lineStrings;
}



int main()
{
    float minX, maxX, minY, maxY;

    auto lineStrings = loadMapData("map_data.json", minX, maxX, minY, maxY);

    auto window = sf::RenderWindow({1920u, 1080u}, "Fortifier: Forge and Conquer");
    window.setFramerateLimit(144);

    // Calculate scaling factors to fit map within the window
    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;

    float scaleX = windowWidth / (maxX - minX);
    float scaleY = windowHeight / (maxY - minY);
    float scale = std::min(scaleX, scaleY);  // Uniform scaling factor

     // Centering offsets
    float offsetX = (windowWidth - (maxX - minX) * scale) / 2.0f;
    float offsetY = (windowHeight - (maxY - minY) * scale) / 2.0f;

    // Initialize the view centered on the map
    sf::View view(sf::FloatRect(0, 0, windowWidth, windowHeight));
    view.setCenter((maxX + minX) / 2.0f * scale + offsetX, (maxY + minY) / 2.0f * scale + offsetY);

    float moveSpeed = 10.0f; // Speed for moving the camera



    while (window.isOpen())
    {
        for (auto event = sf::Event(); window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // Camera movement with arrow keys or WASD
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            view.move(-moveSpeed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            view.move(moveSpeed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            view.move(0, -moveSpeed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            view.move(0, moveSpeed);
        }

        // Apply the view to the window
        window.setView(view);

        window.clear();

         // Draw each LineString
        for (const auto &line : lineStrings) {
            sf::VertexArray lineShape(sf::LineStrip, line.points.size());
            for (size_t i = 0; i < line.points.size(); ++i) {
                // Apply scaling and offset
                float x = (line.points[i].x - minX) * scale + offsetX;
                float y = (line.points[i].y - minY) * scale + offsetY;
                lineShape[i].position = sf::Vector2f(x, y);
                lineShape[i].color = sf::Color::Green; // Set color for visibility
            }
            window.draw(lineShape);
        }
        window.display();
    }
}

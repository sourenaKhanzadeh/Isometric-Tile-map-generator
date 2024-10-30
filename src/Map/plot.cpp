#include "plot.hpp"

PlotOnMap::PlotOnMap() {
}

void PlotOnMap::draw(sf::RenderWindow& window, float zoomFactor) {
    if (!togglePlotMap) {
        return;
    }
    // draw the coordinates on the map
    for (const auto& coordinate : coordinates) {
        sf::CircleShape circle(5);
        circle.setPosition(coordinate.x, coordinate.y);
        circle.setFillColor(sf::Color::Red);
        circle.setOutlineColor(sf::Color::Black);
        circle.setOutlineThickness(1);
        circle.setRadius(5);
        // make a line and its previous point
        if (coordinates.size() > 1) {
            sf::Vertex line[] = {
                sf::Vertex(coordinates[coordinates.size() - 2], sf::Color::Red),
                sf::Vertex(coordinate, sf::Color::Red)
            };
            window.draw(line, 2, sf::Lines);
        }
        window.draw(circle);
    }
}

PlotOnMap::~PlotOnMap() {
    coordinates.clear();
}

void PlotOnMap::addCoordinate(sf::Vector2f coordinate) {
    coordinates.push_back(coordinate);
}

void PlotOnMap::clearCoordinates() {
    coordinates.clear();
}

PlotOnMap::PlotOnMap(std::vector<sf::Vector2f> coordinates) {
    this->coordinates = coordinates;
}

void PlotOnMap::saveCoordinates(const std::string& filename, std::string countryName) {
    // Trim whitespace from country name
    countryName = trim(countryName);

    // Construct JSON data
    nlohmann::json json;
    json["country"] = countryName;
    for (const auto& coord : coordinates) {
        json["coordinates"].push_back({{"x", coord.x}, {"y", coord.y}});
    }

    // Ensure directory structure exists
    std::string basePath = "res/json/countries";
    if (!std::filesystem::exists(basePath)) {
        std::filesystem::create_directories(basePath);
    }
    std::string countryPath = basePath + "/" + countryName;
    if (!std::filesystem::exists(countryPath)) {
        std::filesystem::create_directories(countryPath);
    }

    // Open file for writing
    std::string fullFilePath = countryPath + "/" + filename;
    std::ofstream file(fullFilePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << fullFilePath << std::endl;
        // Ensure the file is created if it does not exist
        std::ofstream createFile(fullFilePath);
        if (!createFile.is_open()) {
            std::cerr << "Error: Could not create file: " << fullFilePath << std::endl;
            return;
        }
        createFile.close(); // Close the created file
        file.open(fullFilePath); // Reopen the original file for writing
    }

    // Write JSON data
    file << json.dump(4); // Pretty print with 4 spaces
    file.close();
    if (file) {
        std::cout << "File saved successfully to: " << fullFilePath << std::endl;
    } else {
        std::cerr << "Error: Failed to write data to file." << std::endl;
    }
}

PlotOnMap *PlotOnMap::createPlotOnMap() {
    return new PlotOnMap();
}

PlotOnMap *PlotOnMap::createPlotOnMap(std::vector<sf::Vector2f> coordinates) {
    return new PlotOnMap(coordinates);
}

void PlotOnMap::setCountryName(std::string countryName) {
    this->countryName = countryName;
}

std::string PlotOnMap::getCountryName() {
    return countryName;
}

void PlotOnMap::handleEvent(sf::Event event, sf::RenderWindow& window) {
    if (!togglePlotMap) {
        return;
    }
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            addCoordinate(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
        }
    }
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
        // remove the last coordinate
        if (coordinates.size() > 0) {
            coordinates.pop_back();
        }
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        togglePlotMap = false;
    }
}


std::string trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t\n\r\f\v");
    auto end = str.find_last_not_of(" \t\n\r\f\v");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}
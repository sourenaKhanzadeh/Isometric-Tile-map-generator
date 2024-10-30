#ifndef CONTOURS_HPP
#define CONTOURS_HPP

#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <mutex>
#include <future>
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>
#include "../Utils/progressBar.hpp"
#include "../Camera/controller.hpp"
class Contours {
private:
    std::vector<std::vector<int>> contours;
    std::string contoursPath;
    sf::Color contourColor = sf::Color::Green;
    void loadContours();
    sf::Vector2f minBounds;
    sf::Vector2f maxBounds;

    void calculateBounds();
public:
    ~Contours();
    Contours(std::string contoursPath);
    std::vector<std::vector<int>> getContours();
    void draw(sf::RenderWindow& window, float zoomFactor);
    void asyncLoadContours(ProgressBar& progressBar);
    void changeColor(sf::Color color);
};

#endif
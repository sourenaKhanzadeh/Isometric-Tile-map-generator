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
#include <iostream>
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
    bool isPointInContour(int x, int y, const std::vector<int>& contour);

    void calculateBounds();
public:
    ~Contours();
    Contours(std::string contoursPath);
    std::vector<std::vector<int>> getContours();
    void draw(sf::RenderWindow& window, float zoomFactor);
    void asyncLoadContours(ProgressBar& progressBar);
    void changeColor(sf::Color color);
    void handleEvents(sf::Event& event);
    void setSelectedContour(std::vector<int> contour);
    void drawSelectedContour(sf::RenderWindow& window, float zoomFactor);
    std::vector<int> selectedContour;
};

#endif
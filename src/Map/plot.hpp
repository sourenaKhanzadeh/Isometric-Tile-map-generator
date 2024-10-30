#ifndef MAP_PLOT_HPP
#define MAP_PLOT_HPP

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>


std::string trim(const std::string& str);

class PlotOnMap {
public:
    PlotOnMap();
    PlotOnMap(std::vector<sf::Vector2f> coordinates);
    ~PlotOnMap();
    void draw(sf::RenderWindow& window, float zoomFactor);
    void addCoordinate(sf::Vector2f coordinate);
    void clearCoordinates();
    void saveCoordinates(const std::string &filename, std::string countryName);
    void setCountryName(std::string countryName);
    std::string getCountryName();
    static PlotOnMap *createPlotOnMap();
    static PlotOnMap *createPlotOnMap(std::vector<sf::Vector2f> coordinates);
    void handleEvent(sf::Event event, sf::RenderWindow& window);

    bool togglePlotMap = false;
private:
    std::vector<sf::Vector2f> coordinates;
    std::string countryName;
};

#endif // MAP_PLOT_HPP

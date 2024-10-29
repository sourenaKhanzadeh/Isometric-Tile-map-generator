// progressbar.hpp
#ifndef PROGRESSBAR_HPP
#define PROGRESSBAR_HPP

#include <SFML/Graphics.hpp>

class ProgressBar {
public:
    ProgressBar(sf::RenderWindow& window, const sf::Vector2f& size, const sf::Vector2f& position, const sf::Color& fillColor, const sf::Color& backgroundColor);

    // Set the total number of items to load
    void setTotalItems(size_t total);

    // Update the progress by increasing the loaded items
    void incrementProgress();

    // Draw the progress bar to the window
    void draw();

    bool isComplete() const;

    void update(const std::string& text);
private:
    size_t totalItems = 1;
    size_t loadedItems = 0;

    sf::RectangleShape backgroundBar;
    sf::RectangleShape progressBar;
    sf::Font font;
    sf::Text loadingText;
    sf::RenderWindow& window;

};
#endif // PROGRESSBAR_HPP
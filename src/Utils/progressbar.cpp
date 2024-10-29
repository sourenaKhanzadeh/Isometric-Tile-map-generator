// progressbar.cpp
#include "progressbar.hpp"

ProgressBar::ProgressBar(sf::RenderWindow& window, const sf::Vector2f& size, const sf::Vector2f& position, const sf::Color& fillColor, const sf::Color& backgroundColor)
    : totalItems(1), loadedItems(0), window(window)
{
    // Set up the background bar
    backgroundBar.setSize(size);
    backgroundBar.setPosition(position);
    backgroundBar.setFillColor(backgroundColor);

    // Set up the progress bar
    progressBar.setSize({0, size.y}); // Start with zero width
    progressBar.setPosition(position);
    progressBar.setFillColor(fillColor);

    // Load font
    if (!font.loadFromFile("res/font/arial.ttf")) {  // Ensure you have a valid font at this path
        throw std::runtime_error("Failed to load font for ProgressBar!");
    }
    
    // Set up loading text
    loadingText.setFont(font);
    loadingText.setString("Loading...");
    loadingText.setCharacterSize(24);
    loadingText.setFillColor(sf::Color::White);
    loadingText.setPosition(position.x, position.y - 40);
}

void ProgressBar::setTotalItems(size_t total) {
    totalItems = total;
}

void ProgressBar::incrementProgress() {
    if (loadedItems < totalItems) {
        ++loadedItems;
        float progressRatio = static_cast<float>(loadedItems) / totalItems;
        progressBar.setSize({backgroundBar.getSize().x * progressRatio, backgroundBar.getSize().y});
    }
}

bool ProgressBar::isComplete() const {
    return loadedItems >= totalItems;
}

void ProgressBar::draw() {
    window.draw(backgroundBar);
    window.draw(progressBar);
    window.draw(loadingText);
}


void ProgressBar::update(const std::string& text) {
    loadingText.setString(text);

    // Display loading screen and update progress bar
     while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }


        window.clear(sf::Color::Black);
        draw();
        window.display();

        if (isComplete()) {
            break;
        }
    }
}

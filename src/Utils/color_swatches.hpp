// color_picker.hpp

#ifndef COLOR_PICKER_HPP
#define COLOR_PICKER_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include "../Map/renderer.hpp"
namespace Utils {

    class ColorPicker {
    private:
        MapRenderer& mapRenderer;

        sf::Font font;
        sf::Text colorHexText;
        sf::RectangleShape colorDisplay;
        sf::RectangleShape redSlider, greenSlider, blueSlider;
        sf::RectangleShape redKnob, greenKnob, blueKnob;
        sf::RectangleShape hexTextBox;
        sf::RenderWindow& window;

        int redValue, greenValue, blueValue;
        bool redActive, greenActive, blueActive, hexInputActive;

        void decodeHex(const std::string& hex);
        std::string encodeHex() const;

    public:
        ColorPicker(sf::RenderWindow& window, MapRenderer& mapRenderer);
        void handleEvent(const sf::Event& event);
        void draw();
        sf::Color getColor() const;
        void setColorHex(const std::string& hex);
    };
}

#endif // COLOR_PICKER_HPP

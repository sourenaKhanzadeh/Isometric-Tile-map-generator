// color_picker.cpp

#include "color_swatches.hpp"
#include <sstream>
#include <iomanip>

namespace Utils {

    ColorPicker::ColorPicker(sf::RenderWindow& window) 
        : window(window), redValue(0), greenValue(0), blueValue(0), 
          redActive(false), greenActive(false), blueActive(false), hexInputActive(false) {

        // Load font for text display
        if (!font.loadFromFile("arial.TTF")) {
            throw std::runtime_error("Failed to load font");
        }

        // Color display setup
        colorDisplay.setSize(sf::Vector2f(100, 100));
        colorDisplay.setPosition(50, 50);
        colorDisplay.setFillColor(sf::Color(redValue, greenValue, blueValue));

        // Slider and knob setup
        const sf::Vector2f sliderSize(255, 10);
        
        redSlider.setSize(sliderSize);
        redSlider.setPosition(50, 200);
        redSlider.setFillColor(sf::Color::Red);
        redKnob.setSize({10, 20});
        redKnob.setFillColor(sf::Color::White);
        redKnob.setPosition(50, 195);

        greenSlider.setSize(sliderSize);
        greenSlider.setPosition(50, 250);
        greenSlider.setFillColor(sf::Color::Green);
        greenKnob.setSize({10, 20});
        greenKnob.setFillColor(sf::Color::White);
        greenKnob.setPosition(50, 245);

        blueSlider.setSize(sliderSize);
        blueSlider.setPosition(50, 300);
        blueSlider.setFillColor(sf::Color::Blue);
        blueKnob.setSize({10, 20});
        blueKnob.setFillColor(sf::Color::White);
        blueKnob.setPosition(50, 295);

        // Hex color text display
        colorHexText.setFont(font);
        colorHexText.setCharacterSize(20);
        colorHexText.setFillColor(sf::Color::White);
        colorHexText.setPosition(145, 355);  // Positioned inside hexTextBox
        colorHexText.setString("000000"); // Default hex value

        // Hex input box for visualization
        hexTextBox.setSize(sf::Vector2f(120, 30));
        hexTextBox.setFillColor(sf::Color(50, 50, 50));  // Dark gray background for visibility
        hexTextBox.setOutlineColor(sf::Color::White);    // White border for better distinction
        hexTextBox.setOutlineThickness(2);
        hexTextBox.setPosition(140, 350);                // Adjusted position to align with colorHexText
    }

    void ColorPicker::handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed) {
            auto mouseX = event.mouseButton.x;
            auto mouseY = event.mouseButton.y;

            // Toggle hex input mode
            if (hexTextBox.getGlobalBounds().contains(mouseX, mouseY)) {
                hexInputActive = true;
            } else {
                hexInputActive = false;
            }

            // Slider knob interaction
            if (redKnob.getGlobalBounds().contains(mouseX, mouseY))
                redActive = true;
            else if (greenKnob.getGlobalBounds().contains(mouseX, mouseY))
                greenActive = true;
            else if (blueKnob.getGlobalBounds().contains(mouseX, mouseY))
                blueActive = true;
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            redActive = greenActive = blueActive = false;
        }
        else if (event.type == sf::Event::MouseMoved) {
            auto mouseX = event.mouseMove.x;
            
            if (redActive) {
                redValue = std::clamp(mouseX - static_cast<int>(redSlider.getPosition().x), 0, 255);
                redKnob.setPosition(redSlider.getPosition().x + redValue, redKnob.getPosition().y);
            }
            if (greenActive) {
                greenValue = std::clamp(mouseX - static_cast<int>(greenSlider.getPosition().x), 0, 255);
                greenKnob.setPosition(greenSlider.getPosition().x + greenValue, greenKnob.getPosition().y);
            }
            if (blueActive) {
                blueValue = std::clamp(mouseX - static_cast<int>(blueSlider.getPosition().x), 0, 255);
                blueKnob.setPosition(blueSlider.getPosition().x + blueValue, blueKnob.getPosition().y);
            }

            // Update color display based on slider values
            colorDisplay.setFillColor(sf::Color(redValue, greenValue, blueValue));
            colorHexText.setString(encodeHex());
        }
        else if (event.type == sf::Event::TextEntered && hexInputActive) {
            if (std::isxdigit(event.text.unicode) && colorHexText.getString().getSize() < 6) {
                colorHexText.setString(colorHexText.getString() + static_cast<char>(event.text.unicode));
            } else if (event.text.unicode == '\b' && colorHexText.getString().getSize() > 0) {  // Backspace
                std::string currentText = colorHexText.getString();
                currentText.pop_back();
                colorHexText.setString(currentText);
            }
            
            if (colorHexText.getString().getSize() == 6) {
                try {
                    setColorHex(colorHexText.getString());
                } catch (const std::exception&) {
                    // Ignore invalid input
                }
            }
        }
    }

    void ColorPicker::draw() {
        window.draw(colorDisplay);
        window.draw(redSlider);
        window.draw(redKnob);
        window.draw(greenSlider);
        window.draw(greenKnob);
        window.draw(blueSlider);
        window.draw(blueKnob);

        window.draw(hexTextBox);
        window.draw(colorHexText);
    }

    sf::Color ColorPicker::getColor() const {
        return sf::Color(redValue, greenValue, blueValue);
    }

    void ColorPicker::setColorHex(const std::string& hex) {
        decodeHex(hex);
        colorDisplay.setFillColor(sf::Color(redValue, greenValue, blueValue));
        colorHexText.setString(encodeHex());
    }

    void ColorPicker::decodeHex(const std::string& hex) {
       if(hex.length() != 6)
           throw std::invalid_argument("Hex must be 6 characters long");

       redValue = std::stoi(hex.substr(0, 2), nullptr, 16);
       greenValue = std::stoi(hex.substr(2, 2), nullptr, 16);
       blueValue = std::stoi(hex.substr(4, 2), nullptr, 16);
    }

    std::string ColorPicker::encodeHex() const {
        std::ostringstream ss;
        ss << std::hex << std::setw(2) << std::setfill('0') << redValue;
        ss << std::setw(2) << std::setfill('0') << greenValue;
        ss << std::setw(2) << std::setfill('0') << blueValue;
        return ss.str();
    }
}

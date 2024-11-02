#ifndef CUBE_HPP
#define CUBE_HPP

#include <SFML/Graphics.hpp>
#include "../Camera/controller.hpp"

class Cube {
public:
    Cube(CameraController cameraController);
    void draw(sf::RenderWindow& window);
    sf::VertexArray cube;

private:
    CameraController cameraController;
};

#endif

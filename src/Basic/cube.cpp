#include "cube.hpp"

Cube::Cube(CameraController cameraController) : cameraController(cameraController) {
    cube = sf::VertexArray(sf::Quads, 12);
}

void Cube::draw(sf::RenderWindow& window) {
    float height = 100;
    // isometric projection
    float isoX = 0;
    float isoY = 0;
    float isoHeight = height / 10;

    // left side
    cube[0].position = sf::Vector2f(isoX, isoY);
    cube[1].position = sf::Vector2f(isoX + height, isoY - isoHeight);
    cube[2].position = sf::Vector2f(isoX + height, isoY + height - isoHeight);
    cube[3].position = sf::Vector2f(isoX, isoY + height);

    // side right
    cube[4].position = sf::Vector2f(isoX + height, isoY - isoHeight);
    cube[5].position = sf::Vector2f(isoX + height * 2, isoY);
    cube[6].position = sf::Vector2f(isoX + height * 2, isoY + height);
    cube[7].position = sf::Vector2f(isoX + height, isoY + height - isoHeight);

    // bottom face
    cube[8].position = sf::Vector2f(isoX, isoY + height);
    cube[9].position = sf::Vector2f(isoX + height, isoY + height - isoHeight);
    cube[10].position = sf::Vector2f(isoX + height * 2, isoY + height);
    cube[11].position = sf::Vector2f(isoX + height, isoY + height + isoHeight);

    for (int i = 0; i < 4; i++) {
        cube[i].color = sf::Color::Red;
    }
    for (int i = 4; i < 8; i++) {
        cube[i].color = sf::Color::Green;
    }
    for (int i = 8; i < 12; i++) {
        cube[i].color = sf::Color::Blue;
    }

    window.draw(cube);
}
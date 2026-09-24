#include "main.hpp"
#include <iostream>

display::display(unsigned int width, unsigned int height)
    : window(sf::VideoMode(width, height), "Voxel Raycaster")
{
}


bool display::isopen() const {
    return window.isOpen();
}

bool display::eventhandler() {
    sf::Event event;

    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                return true;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Left) {
                    std::cout << "Hello World" << std::endl;
                }
                break;

            default:
                break;
        }
    }

    return false;
}



void display::clear(const sf::Color& color) {
    window.clear(color);
}

void display::render() {
    window.display();
}
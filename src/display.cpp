#include "main.hpp"
#include <iostream>

display::display(unsigned int width, unsigned int height)
    : window(sf::VideoMode(width, height), "Voxel Raycaster"),
      pixel(width * height, sf::Color::Black),
      width(width),
      height(height)
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

void display::colorpixel(int x, int y, const sf::Color& color) {
    if (x < 0 || y < 0 || x >= static_cast<int>(width) || y >= static_cast<int>(height)) {
        return;
    }

    pixel[static_cast<std::size_t>(y) * width + static_cast<std::size_t>(x)] = color;
}

void display::draw() {
    sf::RectangleShape rectangle;
    rectangle.setSize({1.0f, 1.0f});

    for (unsigned int i = 0; i < width; ++i) {
        for (unsigned int j = 0; j < height; ++j) {
            rectangle.setPosition({static_cast<float>(i), static_cast<float>(j)});
            rectangle.setFillColor(pixel[j * width + i]);
            window.draw(rectangle);
        }
    }
}

void display::fillpixel(unsigned int x, unsigned int y) {
    for (unsigned int i = 0; i < x; ++i) {
        for (unsigned int j = 0; j < y; ++j) {
            colorpixel(static_cast<int>(i), static_cast<int>(j), sf::Color::White);
        }
    }
}
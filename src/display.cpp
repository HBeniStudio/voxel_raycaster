#include "main.hpp"

display::display(unsigned int width, unsigned int height)
    : window(sf::VideoMode(width, height), "My window")
{
}

void display::destroy() {
    window.close();
}

bool display::isopen() {
    return window.isOpen();
}
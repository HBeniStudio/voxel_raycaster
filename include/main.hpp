#pragma once
#include <string>
#include <SFML/Graphics.hpp>

class display {
public:
    display(unsigned int width, unsigned int height);
    void destroy();
    bool isopen();

private:
    sf::RenderWindow window;
};

#pragma once
#include <string>
#include <SFML/Graphics.hpp>

class display {
public:
    display(unsigned int width, unsigned int height);
    bool isopen() const;
    bool eventhandler();
    void clear(const sf::Color& color = sf::Color::Black);
    void render();

private:
    sf::RenderWindow window;
};

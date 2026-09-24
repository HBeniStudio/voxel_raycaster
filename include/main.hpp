#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <vector>

class display {
public:
    display(unsigned int width, unsigned int height);

    bool isopen() const;
    bool eventhandler();
    void clear(const sf::Color& color = sf::Color::Black);
    void render();
    void colorpixel(int x, int y, const sf::Color& color);
    void draw();
    void fillpixel(unsigned int x, unsigned int y);

private:
    sf::RenderWindow window;
    std::vector<sf::Color> pixel;
    unsigned int width;
    unsigned int height;
};

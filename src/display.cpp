#include "main.hpp"

void display::destroy(){

}

display::display(unsigned int height, unsigned int width){
    sf::Window window(sf::VideoMode({height, width}), "My window");
}
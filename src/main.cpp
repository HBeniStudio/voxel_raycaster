#include <filesystem>
#include <iostream>
#include "main.hpp"

int main() {
    display display_window(1280, 720);
    display_window.fillpixel(1280, 720);
    
    for(int i = 100;i<200;i++){
        for(int j = 100;j<200;j++){
            display_window.colorpixel(i,j,sf::Color::Black);
        }
    }


    while (display_window.isopen()) {
        if (display_window.eventhandler()) {
            break;
        }

        display_window.clear(sf::Color::Black);
        display_window.draw();
        display_window.render();
    }

    return 0;
}

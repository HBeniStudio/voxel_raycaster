#include <filesystem>
#include <iostream>
#include "main.hpp"

int main() {
    display display_window(1280, 720);

    while (display_window.isopen()) {
        if (display_window.eventhandler()) {
            break;
        }

        display_window.clear(sf::Color::Black);
        display_window.render();
    }

    return 0;
}

#include <filesystem>
#include <iostream>
#include "main.hpp"

int screen_width = 1280;
int screen_height = 720;


int main() {
    display display_window(screen_width, screen_height);
    float camera_x = 128.0f;
    float camera_y = 128.0f;
    float camera_z = 110.0f;
    const float move_speed = 2.5f;
    const float up_down_speed = 4.0f;

    while (display_window.isopen()) {
        if (display_window.eventhandler()) {
            break;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            camera_y -= move_speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            camera_y += move_speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            camera_x -= move_speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            camera_x += move_speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            camera_z += up_down_speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            camera_z -= up_down_speed;
        }

        display_window.game_draw({camera_x, camera_y, camera_z}, screen_width, screen_height);
        display_window.clear(sf::Color::Black);
        display_window.draw();
        display_window.render();
    }

    return 0;
}

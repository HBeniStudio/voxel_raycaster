#include "main.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace {

sf::Color make_terrain_color(float height_value) {
    const float h = std::clamp(height_value, 0.0f, 255.0f);

    if (h < 40.0f) {
        return sf::Color(25, 65, 30);
    }
    if (h < 80.0f) {
        return sf::Color(55, 110, 45);
    }
    if (h < 130.0f) {
        return sf::Color(105, 145, 75);
    }
    if (h < 180.0f) {
        return sf::Color(150, 120, 85);
    }
    if (h < 220.0f) {
        return sf::Color(180, 165, 130);
    }
    if (h < 245.0f) {
        return sf::Color(210, 200, 190);
    }
    return sf::Color(240, 240, 240);
}

sf::Color apply_fog(const sf::Color& base, float fog_strength) {
    const float fog = std::clamp(fog_strength, 0.0f, 1.0f);
    const sf::Color fog_color(255, 230, 120);

    return sf::Color(
        static_cast<sf::Uint8>(base.r * (1.0f - fog) + fog_color.r * fog),
        static_cast<sf::Uint8>(base.g * (1.0f - fog) + fog_color.g * fog),
        static_cast<sf::Uint8>(base.b * (1.0f - fog) + fog_color.b * fog)
    );
}

void generate_debug_maps(sf::Image& heightmap, sf::Image& colormap) {
    const unsigned int map_size = 256;
    heightmap.create(map_size, map_size, sf::Color::Black);
    colormap.create(map_size, map_size, sf::Color::Black);

    for (unsigned int y = 0; y < map_size; ++y) {
        for (unsigned int x = 0; x < map_size; ++x) {
            const float nx = (static_cast<float>(x) / static_cast<float>(map_size - 1)) * 2.0f - 1.0f;
            const float ny = (static_cast<float>(y) / static_cast<float>(map_size - 1)) * 2.0f - 1.0f;
            const float ridge = std::sin(nx * 7.0f) * 0.5f + 0.5f;
            const float valley = std::cos(ny * 6.0f + 2.0f) * 0.5f + 0.5f;
            const float radial = 1.0f - std::min(1.0f, std::sqrt(nx * nx + ny * ny) * 1.3f);
            const float waves = std::sin((nx + ny) * 12.0f) * 0.5f + 0.5f;
            const float height_value = std::clamp((ridge * 0.45f + valley * 0.35f + radial * 0.8f + waves * 0.35f) * 255.0f, 0.0f, 255.0f);

            const sf::Uint8 level = static_cast<sf::Uint8>(height_value);
            const sf::Color gray(level, level, level);
            heightmap.setPixel(x, y, gray);
            colormap.setPixel(x, y, make_terrain_color(height_value));
        }
    }
}

} // namespace

display::display(unsigned int width, unsigned int height)
    : window(sf::VideoMode(width, height), "Voxel Raycaster"),
      pixel(width * height, sf::Color::Black),
      width(width),
      height(height)
{
    generate_debug_maps(heightmap, colormap);
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

void display::game_draw(std::vector<float> p, int width, int height) {
    if (p.size() < 2) {
        return;
    }

    if (heightmap.getSize().x == 0 || heightmap.getSize().y == 0 || colormap.getSize().x == 0 || colormap.getSize().y == 0) {
        return;
    }

    const int map_w = static_cast<int>(colormap.getSize().x);
    const int map_h = static_cast<int>(colormap.getSize().y);
    const float camera_x = p[0];
    const float camera_y = p[1];
    const int horizon = static_cast<int>(height * 0.52f);
    const float camera_height = p.size() > 2 ? p[2] : 110.0f;
    const float scale_height = 520.0f;
    const float max_distance = 700.0f;

    std::fill(pixel.begin(), pixel.end(), sf::Color(135, 206, 235));

    for (float z = max_distance; z > 1.0f; z -= 1.5f) {
        float pleft_x = -z + camera_x;
        float pleft_y = -z + camera_y;
        float pright_x = z + camera_x;
        float pright_y = -z + camera_y;

        const float dx = (pright_x - pleft_x) / static_cast<float>(width);

        for (int x = 0; x < width; ++x) {
            int map_i = static_cast<int>(pleft_x);
            int map_j = static_cast<int>(pleft_y);

            map_i = ((map_i % map_w) + map_w) % map_w;
            map_j = ((map_j % map_h) + map_h) % map_h;

            const float terrain_height = getheight(heightmap, map_i, map_j);
            const float screen_y = (camera_height - terrain_height) / z * scale_height + static_cast<float>(horizon);
            const sf::Color terrain_color = getcolor(colormap, map_i, map_j);
            const sf::Color fogged_color = apply_fog(terrain_color, std::clamp((z / max_distance) * 0.9f, 0.0f, 1.0f));

            for (int y = static_cast<int>(screen_y); y < height; ++y) {
                colorpixel(x, y, fogged_color);
            }

            pleft_x += dx;
            pleft_y = -z + camera_y;
        }
    }
}

float display::getheight(sf::Image& map, int x, int y) {
    if (x < 0 || y < 0 || x >= static_cast<int>(map.getSize().x) || y >= static_cast<int>(map.getSize().y)) {
        return 0.0f;
    }

    sf::Color dot = map.getPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y));
    return dot.r / 255.0f * 100.0f;
}

sf::Color display::getcolor(sf::Image& map, int x, int y) {
    if (x < 0 || y < 0 || x >= static_cast<int>(map.getSize().x) || y >= static_cast<int>(map.getSize().y)) {
        return sf::Color::White;
    }

    return map.getPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y));
}
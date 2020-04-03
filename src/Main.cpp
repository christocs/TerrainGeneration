#include <SFML/Window.hpp>
#include <iostream>

auto main() -> int {
    auto window = sf::Window{sf::VideoMode(800, 600), "cmake-example"};

    while (window.isOpen()) {
        auto event = sf::Event{};

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }
}


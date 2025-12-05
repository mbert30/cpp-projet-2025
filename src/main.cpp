#include <iostream>
#include <exception>

import core.Game;

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    try {
        std::cout << "==================================\n";
        std::cout << "===Platformer Game - C++ Modern===\n";
        std::cout << "==================================\n\n";

        core::Game game(800, 600, "Platformer Game");

        game.run();

        std::cout << "\nGame terminated successfully.\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred.\n";
        return 1;
    }
}
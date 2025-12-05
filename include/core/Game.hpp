#pragma once

#include <SDL.h>
#include <memory>
#include <string>
#include <chrono>

namespace core {

/**
 * @brief Main Game class managing the game loop and window
 *
 * This class follows RAII principles and uses modern C++ features.
 * It manages the main game loop, event handling, and rendering.
 */
class Game {
public:
    /**
     * @brief Construct a new Game object
     *
     * @param width Window width in pixels
     * @param height Window height in pixels
     * @param title Window title
     */
    Game(unsigned int width = 800, unsigned int height = 600,
         const std::string& title = "Platformer Game");

    // Delete copy constructor and assignment operator (single instance)
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // Default move constructor and assignment operator
    Game(Game&&) = default;
    Game& operator=(Game&&) = default;

    ~Game();

    /**
     * @brief Start the game loop
     *
     * This is the main entry point that runs the game until the window is closed.
     */
    void run();

private:
    /**
     * @brief Process all window events
     */
    void processEvents();

    /**
     * @brief Update game logic
     *
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime);

    /**
     * @brief Render everything to the window
     */
    void render();

private:
    // SDL2 window and renderer (using custom deleters for RAII)
    struct SDL_Deleter {
        void operator()(SDL_Window* w) const { if(w) SDL_DestroyWindow(w); }
        void operator()(SDL_Renderer* r) const { if(r) SDL_DestroyRenderer(r); }
    };

    std::unique_ptr<SDL_Window, SDL_Deleter> m_window;
    std::unique_ptr<SDL_Renderer, SDL_Deleter> m_renderer;

    bool m_isRunning;
    std::chrono::high_resolution_clock::time_point m_lastTime;

    // Frame rate management
    static constexpr unsigned int TARGET_FPS = 60;
    static constexpr float TIME_PER_FRAME = 1.0f / TARGET_FPS;
};

} // namespace core

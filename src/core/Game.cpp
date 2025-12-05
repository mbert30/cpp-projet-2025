#include "core/Game.hpp"
#include <iostream>

namespace core {

Game::Game(unsigned int width, unsigned int height, const std::string& title)
    : m_window(nullptr)
    , m_renderer(nullptr)
    , m_isRunning(true)
    , m_lastTime(std::chrono::high_resolution_clock::now())
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("SDL initialization failed: " + std::string(SDL_GetError()));
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        static_cast<int>(width),
        static_cast<int>(height),
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        SDL_Quit();
        throw std::runtime_error("Window creation failed: " + std::string(SDL_GetError()));
    }

    // Create renderer with VSync
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Renderer creation failed: " + std::string(SDL_GetError()));
    }

    // Transfer ownership to unique_ptr
    m_window.reset(window);
    m_renderer.reset(renderer);

    // Get SDL version
    SDL_version version;
    SDL_GetVersion(&version);

    std::cout << "Game initialized: " << width << "x" << height << "\n";
    std::cout << "SDL Version: "
              << static_cast<int>(version.major) << "."
              << static_cast<int>(version.minor) << "."
              << static_cast<int>(version.patch) << "\n";
}

Game::~Game() {
    // unique_ptr will automatically clean up window and renderer
    SDL_Quit();
    std::cout << "SDL cleaned up.\n";
}

void Game::run() {
    std::cout << "Starting game loop...\n";

    while (m_isRunning) {
        // Calculate delta time using chrono
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - m_lastTime).count();
        m_lastTime = currentTime;

        // Limit delta time to avoid large jumps
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }

        processEvents();
        update(deltaTime);
        render();
    }

    std::cout << "Game loop ended.\n";
}

void Game::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                std::cout << "Window close requested.\n";
                m_isRunning = false;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    std::cout << "Escape key pressed - exiting.\n";
                    m_isRunning = false;
                }
                break;

            default:
                break;
        }
    }
}

void Game::update([[maybe_unused]] float deltaTime) {
    // Game logic will be implemented here
    // For now, the game just runs an empty loop
}

void Game::render() {
    // Clear window with a pleasant blue color (cornflower blue: RGB 100, 149, 237)
    SDL_SetRenderDrawColor(m_renderer.get(), 100, 149, 237, 255);
    SDL_RenderClear(m_renderer.get());

    // Draw game objects here (none yet)

    // Present the rendered frame
    SDL_RenderPresent(m_renderer.get());
}

} // namespace core

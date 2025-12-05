module;

#include <SDL.h>
#include <memory>
#include <string>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

export module core.Game;
import entities.Player;
import world.Level;
import world.LevelData;
import world.LevelManager;
import core.GameState;
import ui.MainMenu;
import ui.LevelSelectMenu;
import ui.LevelEditor;
import ui.OptionsMenu;
import ui.VictoryScreen;
import ui.TextRenderer;

export namespace core {

class Game {
public:
    Game(unsigned int width = 800, unsigned int height = 600,
         const std::string& title = "Platformer Game");

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    Game(Game&&) = default;
    Game& operator=(Game&&) = default;

    ~Game();

    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

private:
    struct SDL_Deleter {
        void operator()(SDL_Window* w) const { if(w) SDL_DestroyWindow(w); }
        void operator()(SDL_Renderer* r) const { if(r) SDL_DestroyRenderer(r); }
    };

    std::unique_ptr<SDL_Window, SDL_Deleter> m_window;
    std::unique_ptr<SDL_Renderer, SDL_Deleter> m_renderer;

    bool m_isRunning;
    std::chrono::high_resolution_clock::time_point m_lastTime;

    GameState m_currentState;
    int m_selectedLevel;
    std::string m_selectedLevelName;
    world::LevelCategory m_selectedLevelCategory;

    std::unique_ptr<ui::MainMenu> m_mainMenu;
    std::unique_ptr<ui::LevelSelectMenu> m_levelSelectMenu;
    std::unique_ptr<ui::LevelEditor> m_levelEditor;
    std::unique_ptr<ui::OptionsMenu> m_optionsMenu;
    std::unique_ptr<ui::VictoryScreen> m_victoryScreen;

    std::unique_ptr<entities::Player> m_player;
    std::unique_ptr<world::Level> m_level;

    unsigned int m_windowWidth;
    unsigned int m_windowHeight;

    int m_coinsCollected;

    bool m_keyLeft;
    bool m_keyRight;
    bool m_keyJump;

    int m_mouseX;
    int m_mouseY;

    bool m_screenShake;
    float m_shakeTimer;
    int m_shakeOffsetX;
    int m_shakeOffsetY;
    int m_windowOriginalX;
    int m_windowOriginalY;

    static constexpr unsigned int TARGET_FPS = 60;
    static constexpr float TIME_PER_FRAME = 1.0f / TARGET_FPS;
};

inline Game::Game(unsigned int width, unsigned int height, const std::string& title)
    : m_window(nullptr)
    , m_renderer(nullptr)
    , m_isRunning(true)
    , m_lastTime(std::chrono::high_resolution_clock::now())
    , m_currentState(GameState::MainMenu)
    , m_selectedLevel(0)
    , m_selectedLevelName("")
    , m_selectedLevelCategory(world::LevelCategory::Default)
    , m_mainMenu(nullptr)
    , m_levelSelectMenu(nullptr)
    , m_levelEditor(nullptr)
    , m_optionsMenu(nullptr)
    , m_victoryScreen(nullptr)
    , m_player(nullptr)
    , m_level(nullptr)
    , m_windowWidth(width)
    , m_windowHeight(height)
    , m_coinsCollected(0)
    , m_keyLeft(false)
    , m_keyRight(false)
    , m_keyJump(false)
    , m_mouseX(0)
    , m_mouseY(0)
    , m_screenShake(false)
    , m_shakeTimer(0.0f)
    , m_shakeOffsetX(0)
    , m_shakeOffsetY(0)
    , m_windowOriginalX(0)
    , m_windowOriginalY(0)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("SDL initialization failed: " + std::string(SDL_GetError()));
    }

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

    m_window.reset(window);
    m_renderer.reset(renderer);

    SDL_version version;
    SDL_GetVersion(&version);

    std::cout << "Game initialized: " << width << "x" << height << "\n";
    std::cout << "SDL Version: "
              << static_cast<int>(version.major) << "."
              << static_cast<int>(version.minor) << "."
              << static_cast<int>(version.patch) << "\n";

    m_mainMenu = std::make_unique<ui::MainMenu>(width, height);
    m_levelSelectMenu = std::make_unique<ui::LevelSelectMenu>(width, height);
    m_levelEditor = std::make_unique<ui::LevelEditor>(width, height);
    m_optionsMenu = std::make_unique<ui::OptionsMenu>(width, height);
    m_victoryScreen = std::make_unique<ui::VictoryScreen>(width, height);
    std::cout << "Menus initialized\n";
}

inline Game::~Game() {
    SDL_Quit();
    std::cout << "SDL cleaned up.\n";
}

inline void Game::run() {
    std::cout << "Starting game loop...\n";

    while (m_isRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - m_lastTime).count();
        m_lastTime = currentTime;

        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }

        processEvents();
        update(deltaTime);
        render();
    }

    std::cout << "Game loop ended.\n";
}

inline void Game::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                std::cout << "Window close requested.\n";
                m_isRunning = false;
                break;

            case SDL_MOUSEMOTION:
                m_mouseX = event.motion.x;
                m_mouseY = event.motion.y;

                if (m_currentState == GameState::MainMenu) {
                    m_mainMenu->handleMouseMove(m_mouseX, m_mouseY);
                } else if (m_currentState == GameState::LevelSelect) {
                    m_levelSelectMenu->handleMouseMove(m_mouseX, m_mouseY);
                } else if (m_currentState == GameState::LevelEditor) {
                    m_levelEditor->handleMouseMove(m_mouseX, m_mouseY);
                } else if (m_currentState == GameState::Options) {
                    m_optionsMenu->handleMouseMove(m_mouseX, m_mouseY);
                } else if (m_currentState == GameState::Victory) {
                    m_victoryScreen->handleMouseMove(m_mouseX, m_mouseY);
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (m_currentState == GameState::MainMenu) {
                        GameState newState = m_mainMenu->handleMouseClick(m_mouseX, m_mouseY);
                        if (newState == GameState::Quit) {
                            m_isRunning = false;
                        } else if (newState == GameState::Playing) {
                            m_level = std::make_unique<world::Level>();
                            int gridSize = m_level->getGridSize();
                            float playerSize = static_cast<float>(gridSize);

                            float playerX, playerY;
                            if (m_level->hasStart()) {
                                playerX = m_level->getStartX();
                                playerY = m_level->getStartY();
                            } else {
                                playerX = static_cast<float>(m_windowWidth) / 2.0f - playerSize / 2.0f;
                                playerY = 0.0f;
                            }

                            m_player = std::make_unique<entities::Player>(playerX, playerY, playerSize, playerSize, static_cast<float>(gridSize));
                            m_coinsCollected = 0;
                            std::cout << "Starting default level (grid: " << gridSize << "px, player size: " << playerSize << "px)\n";
                        }
                        m_currentState = newState;
                    } else if (m_currentState == GameState::LevelSelect) {
                        GameState newState = m_levelSelectMenu->handleMouseClick(m_mouseX, m_mouseY, m_selectedLevel, m_selectedLevelName, m_selectedLevelCategory);
                        if (newState == GameState::Playing) {
                            int gridSize = 32;
                            if (!m_selectedLevelName.empty()) {
                                world::LevelManager& manager = world::LevelManager::getInstance();
                                world::LevelData levelData = manager.loadLevel(m_selectedLevelName, m_selectedLevelCategory);
                                gridSize = levelData.gridSize;
                                m_level = std::make_unique<world::Level>(levelData);
                                std::cout << "Loading level: " << m_selectedLevelName
                                          << " from " << (m_selectedLevelCategory == world::LevelCategory::Default ? "default" : "custom")
                                          << " category (grid: " << gridSize << "px)\n";
                            } else {
                                m_level = std::make_unique<world::Level>();
                                std::cout << "Loading default level\n";
                            }

                            float playerSize = static_cast<float>(gridSize);

                            float playerX, playerY;
                            if (m_level->hasStart()) {
                                playerX = m_level->getStartX();
                                playerY = m_level->getStartY();
                                std::cout << "Player spawning at start position (" << playerX << ", " << playerY << ")\n";
                            } else {
                                playerX = static_cast<float>(m_windowWidth) / 2.0f - playerSize / 2.0f;
                                playerY = 0.0f;
                                std::cout << "No start position defined, using default\n";
                            }

                            m_player = std::make_unique<entities::Player>(playerX, playerY, playerSize, playerSize, static_cast<float>(gridSize));
                            m_coinsCollected = 0;
                            std::cout << "Starting level " << m_selectedLevel << " (grid: " << gridSize << "px, player size: " << playerSize << "px)\n";
                        }
                        m_currentState = newState;
                    } else if (m_currentState == GameState::LevelEditor) {
                        m_currentState = m_levelEditor->handleMouseClick(m_mouseX, m_mouseY);
                    } else if (m_currentState == GameState::Options) {
                        m_currentState = m_optionsMenu->handleMouseClick(m_mouseX, m_mouseY);
                    } else if (m_currentState == GameState::Victory) {
                        GameState newState = m_victoryScreen->handleMouseClick(m_mouseX, m_mouseY);
                        if (newState == GameState::Playing) {
                            if (!m_selectedLevelName.empty()) {
                                world::LevelManager& manager = world::LevelManager::getInstance();
                                world::LevelData levelData = manager.loadLevel(m_selectedLevelName, m_selectedLevelCategory);
                                int gridSize = levelData.gridSize;
                                m_level = std::make_unique<world::Level>(levelData);

                                float playerSize = static_cast<float>(gridSize);
                                float playerX, playerY;
                                if (m_level->hasStart()) {
                                    playerX = m_level->getStartX();
                                    playerY = m_level->getStartY();
                                } else {
                                    playerX = static_cast<float>(m_windowWidth) / 2.0f - playerSize / 2.0f;
                                    playerY = 0.0f;
                                }
                                m_player = std::make_unique<entities::Player>(playerX, playerY, playerSize, playerSize, static_cast<float>(gridSize));
                                m_coinsCollected = 0;
                            }
                        }
                        m_currentState = newState;
                    }
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (m_currentState == GameState::LevelEditor) {
                        m_levelEditor->handleMouseRelease(m_mouseX, m_mouseY);
                    }
                }
                break;

            case SDL_KEYDOWN:
                if (m_currentState == GameState::Playing) {
                    SDL_Keycode leftKey = m_optionsMenu->getLeftKey();
                    SDL_Keycode rightKey = m_optionsMenu->getRightKey();
                    SDL_Keycode jumpKey = m_optionsMenu->getJumpKey();

                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        std::cout << "Returning to main menu.\n";
                        m_currentState = GameState::MainMenu;
                        m_player.reset();
                        m_level.reset();
                    } else if (event.key.keysym.sym == leftKey) {
                        m_keyLeft = true;
                    } else if (event.key.keysym.sym == rightKey) {
                        m_keyRight = true;
                    } else if (event.key.keysym.sym == jumpKey) {
                        m_keyJump = true;
                    }
                } else if (m_currentState == GameState::Options) {
                    m_optionsMenu->handleKeyPress(event.key.keysym.sym);
                } else {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        if (m_currentState == GameState::MainMenu) {
                            std::cout << "Escape key pressed - exiting.\n";
                            m_isRunning = false;
                        } else {
                            m_currentState = GameState::MainMenu;
                        }
                    }
                }
                break;

            case SDL_KEYUP:
                if (m_currentState == GameState::Playing) {
                    SDL_Keycode leftKey = m_optionsMenu->getLeftKey();
                    SDL_Keycode rightKey = m_optionsMenu->getRightKey();
                    SDL_Keycode jumpKey = m_optionsMenu->getJumpKey();

                    if (event.key.keysym.sym == leftKey) {
                        m_keyLeft = false;
                    } else if (event.key.keysym.sym == rightKey) {
                        m_keyRight = false;
                    } else if (event.key.keysym.sym == jumpKey) {
                        m_keyJump = false;
                    }
                }
                break;

            default:
                break;
        }
    }
}

inline void Game::update(float deltaTime) {
    if (m_shakeTimer > 0.0f) {
        m_shakeTimer -= deltaTime;
        if (m_shakeTimer <= 0.0f) {
            m_screenShake = false;
            m_shakeOffsetX = 0;
            m_shakeOffsetY = 0;
        }
    }

    if (m_currentState == GameState::Playing && m_player && m_level) {
        m_player->handleInput(m_keyLeft, m_keyRight, m_keyJump);

        float platformY = -1.0f;
        m_level->checkPlatformCollision(
            m_player->getX(),
            m_player->getY(),
            m_player->getWidth(),
            m_player->getHeight(),
            m_player->getVelocityY(),
            platformY
        );

        m_player->update(deltaTime, static_cast<int>(m_windowHeight), platformY);

        if (m_level->checkDeadlyCollision(
            m_player->getX(),
            m_player->getY(),
            m_player->getWidth(),
            m_player->getHeight())) {
            std::cout << "Player died! Restarting level...\n";

            if (!m_selectedLevelName.empty()) {
                world::LevelManager& manager = world::LevelManager::getInstance();
                world::LevelData levelData = manager.loadLevel(m_selectedLevelName, m_selectedLevelCategory);
                int gridSize = levelData.gridSize;
                m_level = std::make_unique<world::Level>(levelData);

                float playerSize = static_cast<float>(gridSize);
                float playerX, playerY;
                if (m_level->hasStart()) {
                    playerX = m_level->getStartX();
                    playerY = m_level->getStartY();
                } else {
                    playerX = static_cast<float>(m_windowWidth) / 2.0f - playerSize / 2.0f;
                    playerY = 0.0f;
                }
                m_player = std::make_unique<entities::Player>(playerX, playerY, playerSize, playerSize, static_cast<float>(gridSize));
                m_coinsCollected = 0;
            }
            return;
        }

        if (m_level->checkUnstableCollision(
            m_player->getX(),
            m_player->getY(),
            m_player->getWidth(),
            m_player->getHeight())) {
            if (!m_screenShake) {
                m_screenShake = true;
                m_shakeTimer = 999.0f;
                SDL_GetWindowPosition(m_window.get(), &m_windowOriginalX, &m_windowOriginalY);
            }
        } else {
            if (m_screenShake && m_shakeTimer > 900.0f) {
                m_screenShake = false;
                m_shakeTimer = 0.0f;
                m_shakeOffsetX = 0;
                m_shakeOffsetY = 0;
                SDL_SetWindowPosition(m_window.get(), m_windowOriginalX, m_windowOriginalY);
            }
        }

        if (m_screenShake) {
            m_shakeOffsetX = (rand() % 11) - 5;
            m_shakeOffsetY = (rand() % 11) - 5;
            SDL_SetWindowPosition(m_window.get(), m_windowOriginalX + m_shakeOffsetX, m_windowOriginalY + m_shakeOffsetY);
        }

        if (m_level->checkCoinCollection(
            m_player->getX(),
            m_player->getY(),
            m_player->getWidth(),
            m_player->getHeight()) != -1) {
            m_coinsCollected++;
            std::cout << "Coin collected! Total: " << m_coinsCollected << "\n";
        }

        if (m_level->checkFinishCollision(
            m_player->getX(),
            m_player->getY(),
            m_player->getWidth(),
            m_player->getHeight())) {
            m_currentState = GameState::Victory;
            std::cout << "Level completed!\n";
        }
    }
}

inline void Game::render() {
    switch (m_currentState) {
        case GameState::MainMenu:
            if (m_mainMenu) {
                m_mainMenu->render(m_renderer.get());
            }
            break;

        case GameState::LevelSelect:
            if (m_levelSelectMenu) {
                m_levelSelectMenu->render(m_renderer.get());
            }
            break;

        case GameState::Playing:
            SDL_SetRenderDrawColor(m_renderer.get(), 100, 149, 237, 255);
            SDL_RenderClear(m_renderer.get());

            if (m_level) {
                m_level->render(m_renderer.get());
            }

            if (m_player) {
                m_player->render(m_renderer.get());
            }

            {
                ui::TextRenderer& textRenderer = ui::TextRenderer::getInstance();
                SDL_Color textColor = {255, 255, 255, 255};
                std::string coinText = "Pieces: " + std::to_string(m_coinsCollected);
                SDL_Texture* coinCounterTexture = textRenderer.renderText(m_renderer.get(), coinText, 24, textColor);

                if (coinCounterTexture) {
                    int textWidth, textHeight;
                    SDL_QueryTexture(coinCounterTexture, nullptr, nullptr, &textWidth, &textHeight);
                    SDL_Rect textRect = {
                        10,
                        10,
                        textWidth,
                        textHeight
                    };
                    SDL_RenderCopy(m_renderer.get(), coinCounterTexture, nullptr, &textRect);
                    SDL_DestroyTexture(coinCounterTexture);
                }
            }
            break;

        case GameState::LevelEditor:
            if (m_levelEditor) {
                m_levelEditor->render(m_renderer.get());
            }
            break;

        case GameState::Options:
            if (m_optionsMenu) {
                m_optionsMenu->render(m_renderer.get());
            }
            break;

        case GameState::Victory:
            SDL_SetRenderDrawColor(m_renderer.get(), 100, 149, 237, 255);
            SDL_RenderClear(m_renderer.get());
            if (m_level) {
                m_level->render(m_renderer.get());
            }
            if (m_player) {
                m_player->render(m_renderer.get());
            }
            if (m_victoryScreen) {
                m_victoryScreen->render(m_renderer.get());
            }
            break;

        case GameState::Quit:
            break;
    }

    SDL_RenderPresent(m_renderer.get());
}

}

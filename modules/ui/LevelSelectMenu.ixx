module;

#include <SDL.h>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

export module ui.LevelSelectMenu;

import ui.Button;
import ui.TextRenderer;
import core.GameState;
import world.LevelManager;

export namespace ui {
    class LevelSelectMenu {
    public:
        LevelSelectMenu(int windowWidth, int windowHeight);

        void handleMouseMove(int x, int y);
        core::GameState handleMouseClick(int x, int y, int& selectedLevel, std::string& selectedLevelName, world::LevelCategory& selectedCategory);
        void handleRightClick(int x, int y);
        void render(SDL_Renderer* renderer) const;
        void refreshLevelList();

    private:
        // Tab buttons
        std::unique_ptr<Button> m_defaultTabButton;
        std::unique_ptr<Button> m_customTabButton;

        // Level buttons
        std::vector<std::unique_ptr<Button>> m_levelButtons;
        std::vector<std::unique_ptr<Button>> m_deleteButtons;  // Delete buttons for custom levels

        // Control buttons
        std::unique_ptr<Button> m_backButton;
        std::unique_ptr<Button> m_refreshButton;

        // Level data
        std::vector<std::string> m_defaultLevelNames;
        std::vector<std::string> m_customLevelNames;

        // UI state
        world::LevelCategory m_currentTab;
        int m_windowWidth;
        int m_scrollOffset;

        void rebuildButtons();
        void switchTab(world::LevelCategory newTab);
    };

    inline LevelSelectMenu::LevelSelectMenu(int windowWidth, int windowHeight)
        : m_currentTab(world::LevelCategory::Default)
        , m_windowWidth(windowWidth)
        , m_scrollOffset(0) {

        // Tab buttons
        float tabWidth = 200.0f;
        float tabHeight = 40.0f;
        float tabY = 20.0f;
        float tabSpacing = 210.0f;
        float tabStartX = static_cast<float>(windowWidth) / 2.0f - tabSpacing;

        m_defaultTabButton = std::make_unique<Button>(tabStartX, tabY, tabWidth, tabHeight, "Classiques");
        m_customTabButton = std::make_unique<Button>(tabStartX + tabSpacing, tabY, tabWidth, tabHeight, "Personnalises");

        // Back button
        m_backButton = std::make_unique<Button>(50.0f, static_cast<float>(windowHeight) - 80.0f, 150.0f, 50.0f, "Retour");

        // Refresh button
        m_refreshButton = std::make_unique<Button>(220.0f, static_cast<float>(windowHeight) - 80.0f, 150.0f, 50.0f, "Actualiser");

        // Load initial level list
        refreshLevelList();
    }

    inline void LevelSelectMenu::refreshLevelList() {
        world::LevelManager& manager = world::LevelManager::getInstance();
        manager.refreshLevelList();
        m_defaultLevelNames = manager.getLevelNames(world::LevelCategory::Default);
        m_customLevelNames = manager.getLevelNames(world::LevelCategory::Custom);
        rebuildButtons();
    }

    inline void LevelSelectMenu::switchTab(world::LevelCategory newTab) {
        m_currentTab = newTab;
        m_scrollOffset = 0;
        rebuildButtons();
    }

    inline void LevelSelectMenu::rebuildButtons() {
        m_levelButtons.clear();
        m_deleteButtons.clear();

        // Get current tab's level names
        const std::vector<std::string>& levelNames =
            (m_currentTab == world::LevelCategory::Default)
                ? m_defaultLevelNames
                : m_customLevelNames;

        float buttonWidth = 300.0f;
        float buttonHeight = 50.0f;
        float startX = static_cast<float>(m_windowWidth) / 2.0f - buttonWidth / 2.0f;
        float startY = 120.0f;
        float spacing = 60.0f;

        // Create a button for each level
        for (size_t i = 0; i < levelNames.size(); ++i) {
            float y = startY + static_cast<float>(i) * spacing - static_cast<float>(m_scrollOffset);
            m_levelButtons.push_back(std::make_unique<Button>(
                startX, y, buttonWidth, buttonHeight, levelNames[i]
            ));

            // Add delete button for custom levels only
            if (m_currentTab == world::LevelCategory::Custom) {
                float deleteButtonX = startX + buttonWidth + 10.0f;
                m_deleteButtons.push_back(std::make_unique<Button>(
                    deleteButtonX, y, 40.0f, buttonHeight, "X"
                ));
            }
        }
    }

    inline void LevelSelectMenu::handleMouseMove(int x, int y) {
        // Update tab buttons hover
        m_defaultTabButton->setHovered(m_defaultTabButton->contains(x, y));
        m_customTabButton->setHovered(m_customTabButton->contains(x, y));

        // Update level buttons hover
        for (auto& button : m_levelButtons) {
            button->setHovered(button->contains(x, y));
        }

        // Update delete buttons hover
        for (auto& button : m_deleteButtons) {
            button->setHovered(button->contains(x, y));
        }

        // Update control buttons hover
        m_backButton->setHovered(m_backButton->contains(x, y));
        m_refreshButton->setHovered(m_refreshButton->contains(x, y));
    }

    inline void LevelSelectMenu::handleRightClick(int x, int y) {
        // Only handle right-click for custom levels
        if (m_currentTab != world::LevelCategory::Custom) {
            return;
        }

        // Check if clicked on a delete button
        for (size_t i = 0; i < m_deleteButtons.size(); ++i) {
            if (m_deleteButtons[i]->contains(x, y)) {
                // Delete the level
                if (i < m_customLevelNames.size()) {
                    const std::string& levelName = m_customLevelNames[i];
                    world::LevelManager& manager = world::LevelManager::getInstance();
                    if (manager.deleteLevel(levelName)) {
                        std::cout << "Deleted level: " << levelName << "\n";
                        refreshLevelList();
                    } else {
                        std::cout << "Failed to delete level: " << levelName << "\n";
                    }
                }
                return;
            }
        }
    }

    inline core::GameState LevelSelectMenu::handleMouseClick(int x, int y, int& selectedLevel,
                                                              std::string& selectedLevelName,
                                                              world::LevelCategory& selectedCategory) {
        // Check tab buttons
        if (m_defaultTabButton->contains(x, y)) {
            switchTab(world::LevelCategory::Default);
            return core::GameState::LevelSelect;
        }
        if (m_customTabButton->contains(x, y)) {
            switchTab(world::LevelCategory::Custom);
            return core::GameState::LevelSelect;
        }

        // Check delete buttons first (for custom levels)
        if (m_currentTab == world::LevelCategory::Custom) {
            for (size_t i = 0; i < m_deleteButtons.size(); ++i) {
                if (m_deleteButtons[i]->contains(x, y)) {
                    // Delete the level
                    if (i < m_customLevelNames.size()) {
                        const std::string& levelName = m_customLevelNames[i];
                        world::LevelManager& manager = world::LevelManager::getInstance();
                        if (manager.deleteLevel(levelName)) {
                            std::cout << "Deleted level: " << levelName << "\n";
                            refreshLevelList();
                        } else {
                            std::cout << "Failed to delete level: " << levelName << "\n";
                        }
                    }
                    return core::GameState::LevelSelect;
                }
            }
        }

        // Check level buttons
        const std::vector<std::string>& levelNames =
            (m_currentTab == world::LevelCategory::Default)
                ? m_defaultLevelNames
                : m_customLevelNames;

        for (size_t i = 0; i < m_levelButtons.size(); ++i) {
            if (m_levelButtons[i]->contains(x, y)) {
                selectedLevel = static_cast<int>(i);
                selectedLevelName = levelNames[i];
                selectedCategory = m_currentTab;
                return core::GameState::Playing;
            }
        }

        // Check refresh button
        if (m_refreshButton->contains(x, y)) {
            refreshLevelList();
            return core::GameState::LevelSelect;
        }

        // Check back button
        if (m_backButton->contains(x, y)) {
            return core::GameState::MainMenu;
        }

        return core::GameState::LevelSelect;
    }

    inline void LevelSelectMenu::render(SDL_Renderer* renderer) const {
        // Background
        SDL_SetRenderDrawColor(renderer, 35, 35, 45, 255);
        SDL_RenderClear(renderer);

        // Render tabs
        // Default tab
        if (m_currentTab == world::LevelCategory::Default) {
            // Active tab (brighter)
            SDL_SetRenderDrawColor(renderer, 80, 80, 120, 255);
        } else {
            // Inactive tab (darker)
            SDL_SetRenderDrawColor(renderer, 50, 50, 70, 255);
        }
        SDL_Rect defaultTabRect = {
            static_cast<int>(m_defaultTabButton->getX()),
            static_cast<int>(m_defaultTabButton->getY()),
            static_cast<int>(m_defaultTabButton->getWidth()),
            static_cast<int>(m_defaultTabButton->getHeight())
        };
        SDL_RenderFillRect(renderer, &defaultTabRect);
        m_defaultTabButton->render(renderer);

        // Custom tab
        if (m_currentTab == world::LevelCategory::Custom) {
            // Active tab (brighter)
            SDL_SetRenderDrawColor(renderer, 80, 80, 120, 255);
        } else {
            // Inactive tab (darker)
            SDL_SetRenderDrawColor(renderer, 50, 50, 70, 255);
        }
        SDL_Rect customTabRect = {
            static_cast<int>(m_customTabButton->getX()),
            static_cast<int>(m_customTabButton->getY()),
            static_cast<int>(m_customTabButton->getWidth()),
            static_cast<int>(m_customTabButton->getHeight())
        };
        SDL_RenderFillRect(renderer, &customTabRect);
        m_customTabButton->render(renderer);

        // Content area background
        SDL_SetRenderDrawColor(renderer, 40, 40, 55, 255);
        SDL_Rect contentRect = {
            50,
            static_cast<int>(m_defaultTabButton->getY() + m_defaultTabButton->getHeight()),
            m_windowWidth - 100,
            450
        };
        SDL_RenderFillRect(renderer, &contentRect);

        // Render level buttons
        for (const auto& button : m_levelButtons) {
            // Only render if visible
            if (button->getY() >= 70 && button->getY() < 500) {
                button->render(renderer);
            }
        }

        // Render delete buttons (for custom levels only)
        if (m_currentTab == world::LevelCategory::Custom) {
            for (const auto& button : m_deleteButtons) {
                // Only render if visible
                if (button->getY() >= 70 && button->getY() < 500) {
                    // Render with red color
                    SDL_SetRenderDrawColor(renderer, 180, 60, 60, 255);
                    SDL_Rect rect = {
                        static_cast<int>(button->getX()),
                        static_cast<int>(button->getY()),
                        static_cast<int>(button->getWidth()),
                        static_cast<int>(button->getHeight())
                    };
                    SDL_RenderFillRect(renderer, &rect);
                    button->render(renderer);
                }
            }
        }

        // Show message if no levels
        const std::vector<std::string>& levelNames =
            (m_currentTab == world::LevelCategory::Default)
                ? m_defaultLevelNames
                : m_customLevelNames;

        if (levelNames.empty()) {
            // Background for message
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_Rect messageRect = {
                m_windowWidth / 2 - 150,
                250,
                300,
                60
            };
            SDL_RenderFillRect(renderer, &messageRect);

            // Render "Aucun niveau" text
            TextRenderer& textRenderer = TextRenderer::getInstance();
            SDL_Color textColor = {255, 255, 255, 255};
            SDL_Texture* emptyText = textRenderer.renderText(renderer, "Aucun niveau", 24, textColor);

            if (emptyText) {
                int textWidth, textHeight;
                SDL_QueryTexture(emptyText, nullptr, nullptr, &textWidth, &textHeight);

                SDL_Rect textRect = {
                    m_windowWidth / 2 - textWidth / 2,
                    250 + 30 - textHeight / 2,
                    textWidth,
                    textHeight
                };

                SDL_RenderCopy(renderer, emptyText, nullptr, &textRect);
                SDL_DestroyTexture(emptyText);
            }
        }

        // Render control buttons
        m_backButton->render(renderer);
        m_refreshButton->render(renderer);
    }
}

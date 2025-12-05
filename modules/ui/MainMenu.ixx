module;

#include <SDL.h>
#include <memory>
#include <vector>

export module ui.MainMenu;

import ui.Button;
import ui.TextRenderer;
import core.GameState;

export namespace ui {
    class MainMenu {
    public:
        MainMenu(int windowWidth, int windowHeight);

        void handleMouseMove(int x, int y);
        core::GameState handleMouseClick(int x, int y);
        void render(SDL_Renderer* renderer) const;

    private:
        std::vector<std::unique_ptr<Button>> m_buttons;
        int m_windowWidth;
    };

    inline MainMenu::MainMenu(int windowWidth, int windowHeight)
        : m_windowWidth(windowWidth) {

        // Calculate button dimensions and positions
        float buttonWidth = 300.0f;
        float buttonHeight = 60.0f;
        float startY = static_cast<float>(windowHeight) / 2.0f - 150.0f;
        float spacing = 80.0f;
        float centerX = static_cast<float>(windowWidth) / 2.0f - buttonWidth / 2.0f;

        // Create buttons
        m_buttons.push_back(std::make_unique<Button>(centerX, startY, buttonWidth, buttonHeight, "Liste des niveaux"));
        m_buttons.push_back(std::make_unique<Button>(centerX, startY + spacing, buttonWidth, buttonHeight, "Editeur"));
        m_buttons.push_back(std::make_unique<Button>(centerX, startY + spacing * 2, buttonWidth, buttonHeight, "Options"));
        m_buttons.push_back(std::make_unique<Button>(centerX, startY + spacing * 3, buttonWidth, buttonHeight, "Quitter"));
    }

    inline void MainMenu::handleMouseMove(int x, int y) {
        for (auto& button : m_buttons) {
            button->setHovered(button->contains(x, y));
        }
    }

    inline core::GameState MainMenu::handleMouseClick(int x, int y) {
        for (const auto& button : m_buttons) {
            if (button->contains(x, y)) {
                const std::string& text = button->getText();
                if (text == "Liste des niveaux") {
                    return core::GameState::LevelSelect;
                } else if (text == "Editeur") {
                    return core::GameState::LevelEditor;
                } else if (text == "Options") {
                    return core::GameState::Options;
                } else if (text == "Quitter") {
                    return core::GameState::Quit;
                }
            }
        }
        return core::GameState::MainMenu; // Stay in main menu if no button clicked
    }

    inline void MainMenu::render(SDL_Renderer* renderer) const {
        // Background
        SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255); // Dark background
        SDL_RenderClear(renderer);

        // Title area
        SDL_SetRenderDrawColor(renderer, 50, 50, 80, 255);
        SDL_Rect titleRect = {
            m_windowWidth / 2 - 200,
            50,
            400,
            80
        };
        SDL_RenderFillRect(renderer, &titleRect);

        // Render title text
        TextRenderer& textRenderer = TextRenderer::getInstance();
        SDL_Color titleColor = {255, 255, 255, 255};
        SDL_Texture* titleTexture = textRenderer.renderText(renderer, "Platformer Game", 48, titleColor);

        if (titleTexture) {
            int textWidth, textHeight;
            SDL_QueryTexture(titleTexture, nullptr, nullptr, &textWidth, &textHeight);
            SDL_Rect textRect = {
                m_windowWidth / 2 - textWidth / 2,
                50 + 40 - textHeight / 2,
                textWidth,
                textHeight
            };
            SDL_RenderCopy(renderer, titleTexture, nullptr, &textRect);
            SDL_DestroyTexture(titleTexture);
        }

        // Render all buttons
        for (const auto& button : m_buttons) {
            button->render(renderer);
        }
    }
}

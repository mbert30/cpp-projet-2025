module;

#include <SDL.h>
#include <memory>
#include <string>

export module ui.VictoryScreen;

import ui.Button;
import ui.TextRenderer;
import core.GameState;

export namespace ui {
    class VictoryScreen {
    public:
        VictoryScreen(int windowWidth, int windowHeight);

        void handleMouseMove(int x, int y);
        core::GameState handleMouseClick(int x, int y);
        void render(SDL_Renderer* renderer) const;

    private:
        std::unique_ptr<Button> m_retryButton;
        std::unique_ptr<Button> m_levelsButton;
        int m_windowWidth;
        int m_windowHeight;
    };

    inline VictoryScreen::VictoryScreen(int windowWidth, int windowHeight)
        : m_windowWidth(windowWidth)
        , m_windowHeight(windowHeight) {

        float buttonWidth = 250.0f;
        float buttonHeight = 60.0f;
        float centerX = static_cast<float>(windowWidth) / 2.0f - buttonWidth / 2.0f;
        float centerY = static_cast<float>(windowHeight) / 2.0f;

        m_retryButton = std::make_unique<Button>(
            centerX, centerY + 20.0f, buttonWidth, buttonHeight, "Rejouer"
        );

        m_levelsButton = std::make_unique<Button>(
            centerX, centerY + 100.0f, buttonWidth, buttonHeight, "Selection des niveaux"
        );
    }

    inline void VictoryScreen::handleMouseMove(int x, int y) {
        m_retryButton->setHovered(m_retryButton->contains(x, y));
        m_levelsButton->setHovered(m_levelsButton->contains(x, y));
    }

    inline core::GameState VictoryScreen::handleMouseClick(int x, int y) {
        if (m_retryButton->contains(x, y)) {
            return core::GameState::Playing;
        }
        if (m_levelsButton->contains(x, y)) {
            return core::GameState::LevelSelect;
        }
        return core::GameState::Victory;
    }

    inline void VictoryScreen::render(SDL_Renderer* renderer) const {
        // Semi-transparent overlay
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_Rect overlay = {0, 0, m_windowWidth, m_windowHeight};
        SDL_RenderFillRect(renderer, &overlay);

        // Victory message background
        SDL_SetRenderDrawColor(renderer, 50, 150, 50, 255);
        SDL_Rect messageRect = {
            m_windowWidth / 2 - 200,
            m_windowHeight / 2 - 100,
            400,
            80
        };
        SDL_RenderFillRect(renderer, &messageRect);

        // Border
        SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
        SDL_RenderDrawRect(renderer, &messageRect);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        // Render victory text
        TextRenderer& textRenderer = TextRenderer::getInstance();
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Texture* victoryText = textRenderer.renderText(renderer, "Niveau termine!", 40, textColor);

        if (victoryText) {
            int textWidth, textHeight;
            SDL_QueryTexture(victoryText, nullptr, nullptr, &textWidth, &textHeight);

            SDL_Rect textRect = {
                m_windowWidth / 2 - textWidth / 2,
                m_windowHeight / 2 - 80,
                textWidth,
                textHeight
            };

            SDL_RenderCopy(renderer, victoryText, nullptr, &textRect);
            SDL_DestroyTexture(victoryText);
        }

        // Render buttons
        m_retryButton->render(renderer);
        m_levelsButton->render(renderer);
    }
}

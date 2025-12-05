module;

#include <SDL.h>
#include <memory>
#include <vector>
#include <string>

export module ui.OptionsMenu;

import ui.Button;
import ui.TextRenderer;
import core.GameState;

export namespace ui {
    struct KeyBinding {
        std::string action;
        SDL_Keycode key;
        float labelX, labelY;
        float buttonX, buttonY;
    };

    class OptionsMenu {
    public:
        OptionsMenu(int windowWidth, int windowHeight);

        void handleMouseMove(int x, int y);
        core::GameState handleMouseClick(int x, int y);
        void handleKeyPress(SDL_Keycode key);
        void render(SDL_Renderer* renderer) const;

        // Getters for key bindings
        SDL_Keycode getLeftKey() const { return m_keyBindings[0].key; }
        SDL_Keycode getRightKey() const { return m_keyBindings[1].key; }
        SDL_Keycode getJumpKey() const { return m_keyBindings[2].key; }

    private:
        std::vector<KeyBinding> m_keyBindings;
        std::vector<std::unique_ptr<Button>> m_keyButtons;
        std::unique_ptr<Button> m_backButton;
        int m_windowWidth;
        int m_windowHeight;
        int m_waitingForKey; // Index of key binding being changed (-1 if none)
    };

    inline OptionsMenu::OptionsMenu(int windowWidth, int windowHeight)
        : m_windowWidth(windowWidth), m_windowHeight(windowHeight), m_waitingForKey(-1) {

        float buttonWidth = 150.0f;
        float buttonHeight = 50.0f;
        float startY = 150.0f;
        float spacing = 80.0f;
        float labelX = static_cast<float>(windowWidth) / 2.0f - 200.0f;
        float buttonX = static_cast<float>(windowWidth) / 2.0f + 50.0f;

        // Initialize default key bindings
        m_keyBindings = {
            {"Gauche", SDLK_LEFT, labelX, startY, buttonX, startY},
            {"Droite", SDLK_RIGHT, labelX, startY + spacing, buttonX, startY + spacing},
            {"Sauter", SDLK_SPACE, labelX, startY + spacing * 2, buttonX, startY + spacing * 2}
        };

        // Create buttons for each key binding
        for (const auto& binding : m_keyBindings) {
            m_keyButtons.push_back(std::make_unique<Button>(
                binding.buttonX, binding.buttonY, buttonWidth, buttonHeight, SDL_GetKeyName(binding.key)
            ));
        }

        // Back button
        m_backButton = std::make_unique<Button>(50.0f, static_cast<float>(windowHeight) - 80.0f, 150.0f, 50.0f, "Retour");
    }

    inline void OptionsMenu::handleMouseMove(int x, int y) {
        for (auto& button : m_keyButtons) {
            button->setHovered(button->contains(x, y));
        }
        m_backButton->setHovered(m_backButton->contains(x, y));
    }

    inline core::GameState OptionsMenu::handleMouseClick(int x, int y) {
        // Check key binding buttons
        for (size_t i = 0; i < m_keyButtons.size(); ++i) {
            if (m_keyButtons[i]->contains(x, y)) {
                m_waitingForKey = static_cast<int>(i);
                return core::GameState::Options;
            }
        }

        // Check back button
        if (m_backButton->contains(x, y)) {
            return core::GameState::MainMenu;
        }

        return core::GameState::Options;
    }

    inline void OptionsMenu::handleKeyPress(SDL_Keycode key) {
        if (m_waitingForKey >= 0 && m_waitingForKey < static_cast<int>(m_keyBindings.size())) {
            m_keyBindings[m_waitingForKey].key = key;
            m_keyButtons[m_waitingForKey] = std::make_unique<Button>(
                m_keyBindings[m_waitingForKey].buttonX,
                m_keyBindings[m_waitingForKey].buttonY,
                150.0f, 50.0f,
                SDL_GetKeyName(key)
            );
            m_waitingForKey = -1;
        }
    }

    inline void OptionsMenu::render(SDL_Renderer* renderer) const {
        // Background
        SDL_SetRenderDrawColor(renderer, 35, 35, 45, 255);
        SDL_RenderClear(renderer);

        // Title area
        SDL_SetRenderDrawColor(renderer, 50, 50, 80, 255);
        SDL_Rect titleRect = {
            m_windowWidth / 2 - 200,
            30,
            400,
            80
        };
        SDL_RenderFillRect(renderer, &titleRect);

        // Render title text
        TextRenderer& textRenderer = TextRenderer::getInstance();
        SDL_Color titleColor = {255, 255, 255, 255};
        SDL_Texture* titleTexture = textRenderer.renderText(renderer, "Controles", 40, titleColor);

        if (titleTexture) {
            int textWidth, textHeight;
            SDL_QueryTexture(titleTexture, nullptr, nullptr, &textWidth, &textHeight);
            SDL_Rect textRect = {
                m_windowWidth / 2 - textWidth / 2,
                30 + 40 - textHeight / 2,
                textWidth,
                textHeight
            };
            SDL_RenderCopy(renderer, titleTexture, nullptr, &textRect);
            SDL_DestroyTexture(titleTexture);
        }

        // Label areas and text
        SDL_SetRenderDrawColor(renderer, 70, 70, 90, 255);
        SDL_Color labelColor = {255, 255, 255, 255};

        for (const auto& binding : m_keyBindings) {
            SDL_Rect labelRect = {
                static_cast<int>(binding.labelX),
                static_cast<int>(binding.labelY),
                150,
                50
            };
            SDL_RenderFillRect(renderer, &labelRect);

            // Render label text
            SDL_Texture* labelTexture = textRenderer.renderText(renderer, binding.action, 24, labelColor);
            if (labelTexture) {
                int textWidth, textHeight;
                SDL_QueryTexture(labelTexture, nullptr, nullptr, &textWidth, &textHeight);
                SDL_Rect textRect = {
                    static_cast<int>(binding.labelX) + 75 - textWidth / 2,
                    static_cast<int>(binding.labelY) + 25 - textHeight / 2,
                    textWidth,
                    textHeight
                };
                SDL_RenderCopy(renderer, labelTexture, nullptr, &textRect);
                SDL_DestroyTexture(labelTexture);
            }
        }

        // Render key buttons
        for (const auto& button : m_keyButtons) {
            button->render(renderer);
        }

        // Render back button
        m_backButton->render(renderer);

        // If waiting for key input, show indicator
        if (m_waitingForKey >= 0) {
            SDL_SetRenderDrawColor(renderer, 200, 200, 50, 255);
            SDL_Rect indicator = {
                m_windowWidth / 2 - 150,
                m_windowHeight - 150,
                300,
                60
            };
            SDL_RenderFillRect(renderer, &indicator);

            // Render waiting message
            SDL_Texture* waitTexture = textRenderer.renderText(renderer, "Appuyez sur une touche...", 24, labelColor);
            if (waitTexture) {
                int textWidth, textHeight;
                SDL_QueryTexture(waitTexture, nullptr, nullptr, &textWidth, &textHeight);
                SDL_Rect textRect = {
                    m_windowWidth / 2 - textWidth / 2,
                    m_windowHeight - 150 + 30 - textHeight / 2,
                    textWidth,
                    textHeight
                };
                SDL_RenderCopy(renderer, waitTexture, nullptr, &textRect);
                SDL_DestroyTexture(waitTexture);
            }
        }
    }
}

module;

#include <SDL.h>
#include <string>

export module ui.Button;

import ui.TextRenderer;

export namespace ui {
    class Button {
    public:
        Button(float x, float y, float width, float height, const std::string& text)
            : m_x(x), m_y(y), m_width(width), m_height(height), m_text(text), m_hovered(false) {}

        void render(SDL_Renderer* renderer) const;
        bool contains(int mouseX, int mouseY) const;
        void setHovered(bool hovered) { m_hovered = hovered; }
        bool isHovered() const { return m_hovered; }
        const std::string& getText() const { return m_text; }
        void setText(const std::string& text) { m_text = text; }

        float getX() const { return m_x; }
        float getY() const { return m_y; }
        float getWidth() const { return m_width; }
        float getHeight() const { return m_height; }

    private:
        float m_x, m_y;
        float m_width, m_height;
        std::string m_text;
        bool m_hovered;
    };

    inline void Button::render(SDL_Renderer* renderer) const {
        // Button background color (different when hovered)
        if (m_hovered) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255); // Light blue when hovered
        } else {
            SDL_SetRenderDrawColor(renderer, 60, 60, 120, 255); // Dark blue
        }

        SDL_Rect rect = {
            static_cast<int>(m_x),
            static_cast<int>(m_y),
            static_cast<int>(m_width),
            static_cast<int>(m_height)
        };
        SDL_RenderFillRect(renderer, &rect);

        // Button border
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Light gray border
        SDL_RenderDrawRect(renderer, &rect);

        // Render text centered on button
        if (!m_text.empty()) {
            TextRenderer& textRenderer = TextRenderer::getInstance();
            SDL_Color textColor = {255, 255, 255, 255}; // White text

            SDL_Texture* textTexture = textRenderer.renderText(renderer, m_text, 20, textColor);
            if (textTexture) {
                int textWidth, textHeight;
                SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);

                // Center text on button
                SDL_Rect textRect = {
                    static_cast<int>(m_x + (m_width - textWidth) / 2),
                    static_cast<int>(m_y + (m_height - textHeight) / 2),
                    textWidth,
                    textHeight
                };

                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_DestroyTexture(textTexture);
            }
        }
    }

    inline bool Button::contains(int mouseX, int mouseY) const {
        return mouseX >= m_x && mouseX <= m_x + m_width &&
               mouseY >= m_y && mouseY <= m_y + m_height;
    }
}

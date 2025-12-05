module;

#include <SDL.h>
#include <cmath>

export module entities.Coin;

export namespace entities {
    class Coin {
    public:
        Coin(float x, float y, float size);

        void render(SDL_Renderer* renderer) const;

        float getX() const { return m_x; }
        float getY() const { return m_y; }
        float getSize() const { return m_size; }
        float getRadius() const { return m_size / 2.0f; }

        float getCenterX() const { return m_x + m_size / 2.0f; }
        float getCenterY() const { return m_y + m_size / 2.0f; }

    private:
        float m_x;
        float m_y;
        float m_size;
    };

    inline Coin::Coin(float x, float y, float size)
        : m_x(x)
        , m_y(y)
        , m_size(size) {
    }

    inline void Coin::render(SDL_Renderer* renderer) const {
        int centerX = static_cast<int>(m_x + m_size / 2.0f);
        int centerY = static_cast<int>(m_y + m_size / 2.0f);
        int radius = static_cast<int>(m_size / 2.0f);

        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);

        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx * dx + dy * dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 200, 160, 0, 255);
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                int distSq = dx * dx + dy * dy;
                if (distSq <= (radius * radius) && distSq >= ((radius - 2) * (radius - 2))) {
                    SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
                }
            }
        }
    }
}

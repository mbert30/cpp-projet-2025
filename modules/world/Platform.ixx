// Global module fragment
module;

#include <SDL.h>

export module world.Platform;

import world.LevelData;

export namespace world {

/**
 * @brief A solid platform that the player can stand on
 */
class Platform {
public:
    /**
     * @brief Construct a platform
     *
     * @param x X position (top-left corner)
     * @param y Y position (top-left corner)
     * @param width Platform width
     * @param height Platform height
     * @param type Platform type (Normal or Deadly)
     */
    Platform(float x, float y, float width, float height, PlatformType type = PlatformType::Normal);

    /**
     * @brief Render the platform
     *
     * @param renderer SDL renderer
     */
    void render(SDL_Renderer* renderer) const;

    // Getters for collision detection
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getWidth() const { return m_width; }
    float getHeight() const { return m_height; }

    /**
     * @brief Get the top Y position of the platform
     */
    float getTop() const { return m_y; }

    /**
     * @brief Get the bottom Y position of the platform
     */
    float getBottom() const { return m_y + m_height; }

    /**
     * @brief Get the left X position of the platform
     */
    float getLeft() const { return m_x; }

    /**
     * @brief Get the right X position of the platform
     */
    float getRight() const { return m_x + m_width; }

    /**
     * @brief Check if platform is deadly
     */
    bool isDeadly() const { return m_type == PlatformType::Deadly; }

    /**
     * @brief Get platform type
     */
    PlatformType getType() const { return m_type; }

private:
    float m_x;
    float m_y;
    float m_width;
    float m_height;
    PlatformType m_type;
};

inline Platform::Platform(float x, float y, float width, float height, PlatformType type)
    : m_x(x)
    , m_y(y)
    , m_width(width)
    , m_height(height)
    , m_type(type)
{
}

inline void Platform::render(SDL_Renderer* renderer) const {
    SDL_Rect rect;
    rect.x = static_cast<int>(m_x);
    rect.y = static_cast<int>(m_y);
    rect.w = static_cast<int>(m_width);
    rect.h = static_cast<int>(m_height);

    if (m_type == PlatformType::Deadly) {
        SDL_SetRenderDrawColor(renderer, 180, 30, 30, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 120, 20, 20, 255);
        SDL_RenderDrawRect(renderer, &rect);
    } else {
        SDL_SetRenderDrawColor(renderer, 139, 90, 43, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 90, 60, 30, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }
}

}

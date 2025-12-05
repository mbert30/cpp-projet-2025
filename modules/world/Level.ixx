// Global module fragment
module;

#include <SDL.h>
#include <vector>
#include <memory>

// Module declaration
export module world.Level;

import world.Platform;
import world.LevelData;
import entities.Coin;

export namespace world {

/**
 * @brief Manages all platforms in the level
 */
class Level {
public:
    Level();  // Default level with some platforms
    explicit Level(const LevelData& levelData);  // Load from LevelData
    explicit Level(const std::string& levelName);  // Load by name

    /**
     * @brief Add a platform to the level
     */
    void addPlatform(float x, float y, float width, float height, PlatformType type = PlatformType::Normal);

    /**
     * @brief Render all platforms
     */
    void render(SDL_Renderer* renderer) const;

    /**
     * @brief Check collision with platforms and return the platform top Y if collision detected
     *
     * @param playerX Player X position
     * @param playerY Player Y position
     * @param playerWidth Player width
     * @param playerHeight Player height
     * @param playerVelocityY Player vertical velocity
     * @param outPlatformY Output: Y position of the platform top if collision detected
     * @return true if collision detected from above
     */
    bool checkPlatformCollision(float playerX, float playerY,
                                float playerWidth, float playerHeight,
                                float playerVelocityY,
                                float& outPlatformY) const;

    // Getters for level data
    int getGridSize() const { return m_gridSize; }
    bool hasStart() const { return m_hasStart; }
    bool hasFinish() const { return m_hasFinish; }
    float getStartX() const { return m_startX; }
    float getStartY() const { return m_startY; }
    float getFinishX() const { return m_finishX; }
    float getFinishY() const { return m_finishY; }

    /**
     * @brief Check if player has reached the finish flag
     */
    bool checkFinishCollision(float playerX, float playerY, float playerWidth, float playerHeight) const;

    /**
     * @brief Check if player touched a deadly platform
     */
    bool checkDeadlyCollision(float playerX, float playerY, float playerWidth, float playerHeight) const;

    /**
     * @brief Check if player is on an unstable platform
     */
    bool checkUnstableCollision(float playerX, float playerY, float playerWidth, float playerHeight) const;

    /**
     * @brief Check if player collected a coin and remove it
     * @return Index of collected coin, or -1 if none
     */
    int checkCoinCollection(float playerX, float playerY, float playerWidth, float playerHeight);

    /**
     * @brief Get number of remaining coins
     */
    size_t getRemainingCoins() const { return m_coins.size(); }

private:
    std::vector<std::unique_ptr<Platform>> m_platforms;
    std::vector<std::unique_ptr<entities::Coin>> m_coins;

    // Level configuration
    int m_gridSize;

    // Start and finish positions
    float m_startX, m_startY;
    float m_finishX, m_finishY;
    bool m_hasStart;
    bool m_hasFinish;
};

// Implementation

inline Level::Level()
    : m_gridSize(32)
    , m_startX(0)
    , m_startY(0)
    , m_finishX(0)
    , m_finishY(0)
    , m_hasStart(false)
    , m_hasFinish(false) {
    // Create a simple test level with a few platforms
    // Ground platform
    addPlatform(0.0f, 550.0f, 800.0f, 50.0f);

    // Floating platforms
    addPlatform(100.0f, 450.0f, 150.0f, 20.0f);
    addPlatform(300.0f, 350.0f, 200.0f, 20.0f);
    addPlatform(550.0f, 450.0f, 150.0f, 20.0f);
    addPlatform(200.0f, 250.0f, 150.0f, 20.0f);
}

inline Level::Level(const LevelData& levelData)
    : m_gridSize(levelData.gridSize)
    , m_startX(levelData.startX)
    , m_startY(levelData.startY)
    , m_finishX(levelData.finishX)
    , m_finishY(levelData.finishY)
    , m_hasStart(levelData.hasStart)
    , m_hasFinish(levelData.hasFinish) {
    // Load platforms from LevelData
    for (const auto& platformData : levelData.platforms) {
        addPlatform(platformData.x, platformData.y, platformData.width, platformData.height, platformData.type);
    }

    // Load coins from LevelData
    float coinSize = static_cast<float>(levelData.gridSize) * 0.6f;
    for (const auto& coinData : levelData.coins) {
        m_coins.push_back(std::make_unique<entities::Coin>(coinData.x, coinData.y, coinSize));
    }
}

inline Level::Level(const std::string& levelName)
    : m_gridSize(32)
    , m_startX(0)
    , m_startY(0)
    , m_finishX(0)
    , m_finishY(0)
    , m_hasStart(false)
    , m_hasFinish(false) {
    // Load level from file by name
    LevelData data;
    data.loadFromFile("levels/" + levelName + ".lvl");

    // Copy level data
    m_gridSize = data.gridSize;
    m_startX = data.startX;
    m_startY = data.startY;
    m_finishX = data.finishX;
    m_finishY = data.finishY;
    m_hasStart = data.hasStart;
    m_hasFinish = data.hasFinish;

    for (const auto& platformData : data.platforms) {
        addPlatform(platformData.x, platformData.y, platformData.width, platformData.height, platformData.type);
    }

    // Load coins
    float coinSize = static_cast<float>(m_gridSize) * 0.6f;
    for (const auto& coinData : data.coins) {
        m_coins.push_back(std::make_unique<entities::Coin>(coinData.x, coinData.y, coinSize));
    }
}

inline void Level::addPlatform(float x, float y, float width, float height, PlatformType type) {
    m_platforms.push_back(std::make_unique<Platform>(x, y, width, height, type));
}

inline void Level::render(SDL_Renderer* renderer) const {
    for (const auto& platform : m_platforms) {
        platform->render(renderer);
    }

    // Render coins
    for (const auto& coin : m_coins) {
        coin->render(renderer);
    }

    // Render finish flag if present (during gameplay, not start position)
    if (m_hasFinish) {
        int flagX = static_cast<int>(m_finishX);
        int flagY = static_cast<int>(m_finishY);
        int cellSize = m_gridSize;

        // Flag pole (black)
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_Rect poleRect = {
            flagX + cellSize / 2 - 2,
            flagY,
            4,
            cellSize
        };
        SDL_RenderFillRect(renderer, &poleRect);

        // Flag (red)
        SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
        SDL_Rect flagRect = {
            flagX + cellSize / 2,
            flagY + cellSize / 4,
            cellSize / 2,
            cellSize / 3
        };
        SDL_RenderFillRect(renderer, &flagRect);
    }
}

inline bool Level::checkPlatformCollision(float playerX, float playerY,
                                          float playerWidth, float playerHeight,
                                          float playerVelocityY,
                                          float& outPlatformY) const {
    // Player bounding box
    float playerLeft = playerX;
    float playerRight = playerX + playerWidth;
    float playerTop = playerY;
    float playerBottom = playerY + playerHeight;

    // Only check collision if player is falling (moving down)
    if (playerVelocityY <= 0.0f) {
        return false;
    }

    for (const auto& platform : m_platforms) {
        float platformLeft = platform->getLeft();
        float platformRight = platform->getRight();
        float platformTop = platform->getTop();

        // Check horizontal overlap
        bool horizontalOverlap = playerRight > platformLeft && playerLeft < platformRight;

        // Check if player is landing on top of platform
        // Player's bottom should be near or below platform's top
        bool verticalCollision = playerBottom >= platformTop && playerTop < platformTop;

        if (horizontalOverlap && verticalCollision) {
            // Make sure player was above the platform in previous frame
            // This prevents collision from below or sides
            float previousPlayerBottom = playerBottom - playerVelocityY * (1.0f/60.0f); // Approximate previous position
            if (previousPlayerBottom <= platformTop) {
                outPlatformY = platformTop;
                return true;
            }
        }
    }

    return false;
}

inline bool Level::checkFinishCollision(float playerX, float playerY, float playerWidth, float playerHeight) const {
    if (!m_hasFinish) {
        return false;
    }

    // Player bounding box
    float playerLeft = playerX;
    float playerRight = playerX + playerWidth;
    float playerTop = playerY;
    float playerBottom = playerY + playerHeight;

    // Finish flag bounding box
    float finishLeft = m_finishX;
    float finishRight = m_finishX + static_cast<float>(m_gridSize);
    float finishTop = m_finishY;
    float finishBottom = m_finishY + static_cast<float>(m_gridSize);

    // Check overlap
    bool horizontalOverlap = playerRight > finishLeft && playerLeft < finishRight;
    bool verticalOverlap = playerBottom > finishTop && playerTop < finishBottom;

    return horizontalOverlap && verticalOverlap;
}

inline bool Level::checkDeadlyCollision(float playerX, float playerY, float playerWidth, float playerHeight) const {
    float playerLeft = playerX;
    float playerRight = playerX + playerWidth;
    float playerTop = playerY;
    float playerBottom = playerY + playerHeight;

    for (const auto& platform : m_platforms) {
        if (platform->isDeadly()) {
            float platformLeft = platform->getLeft();
            float platformRight = platform->getRight();
            float platformTop = platform->getTop();
            float platformBottom = platform->getBottom();

            bool horizontalOverlap = playerRight > platformLeft && playerLeft < platformRight;
            bool verticalOverlap = playerBottom > platformTop && playerTop < platformBottom;

            if (horizontalOverlap && verticalOverlap) {
                return true;
            }
        }
    }

    return false;
}

inline bool Level::checkUnstableCollision(float playerX, float playerY, float playerWidth, float playerHeight) const {
    float playerLeft = playerX;
    float playerRight = playerX + playerWidth;
    float playerBottom = playerY + playerHeight;

    for (const auto& platform : m_platforms) {
        if (platform->isUnstable()) {
            float platformLeft = platform->getLeft();
            float platformRight = platform->getRight();
            float platformTop = platform->getTop();

            bool horizontalOverlap = playerRight > platformLeft && playerLeft < platformRight;
            bool onPlatform = playerBottom >= platformTop && playerBottom <= platformTop + 5.0f;

            if (horizontalOverlap && onPlatform) {
                return true;
            }
        }
    }

    return false;
}

inline int Level::checkCoinCollection(float playerX, float playerY, float playerWidth, float playerHeight) {
    // Player center point for collision with coins
    float playerCenterX = playerX + playerWidth / 2.0f;
    float playerCenterY = playerY + playerHeight / 2.0f;

    // Check collision with each coin
    for (size_t i = 0; i < m_coins.size(); ++i) {
        const auto& coin = m_coins[i];
        float coinCenterX = coin->getCenterX();
        float coinCenterY = coin->getCenterY();
        float coinRadius = coin->getRadius();

        // Calculate distance between player center and coin center
        float dx = playerCenterX - coinCenterX;
        float dy = playerCenterY - coinCenterY;
        float distanceSq = dx * dx + dy * dy;
        float collisionRadius = coinRadius + (playerWidth / 2.0f);

        // Check if collision occurred
        if (distanceSq <= collisionRadius * collisionRadius) {
            // Remove the coin
            m_coins.erase(m_coins.begin() + static_cast<long>(i));
            return static_cast<int>(i);
        }
    }

    return -1;
}

} // namespace world

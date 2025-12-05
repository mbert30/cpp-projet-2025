// Global module fragment
module;

#include <SDL.h>
#include <cmath>

// Module declaration
export module entities.Player;

export namespace entities {

/**
 * @brief Player character with movement and physics
 *
 * Handles player input, movement, jumping, and collision with ground.
 */
class Player {
public:
    /**
     * @brief Construct a new Player object
     *
     * @param x Initial x position
     * @param y Initial y position
     * @param width Player width in pixels
     * @param height Player height in pixels
     * @param gridSize Size of grid cell (for physics scaling)
     */
    Player(float x, float y, float width = 32.0f, float height = 32.0f, float gridSize = 32.0f);

    /**
     * @brief Update player physics and movement
     *
     * @param deltaTime Time elapsed since last update
     * @param windowHeight Height of the game window (for ground collision)
     * @param platformY Y position of platform if collision detected, -1.0f if no collision
     */
    void update(float deltaTime, int windowHeight, float platformY = -1.0f);

    /**
     * @brief Render the player to the screen
     *
     * @param renderer SDL renderer to draw to
     */
    void render(SDL_Renderer* renderer) const;

    /**
     * @brief Handle keyboard input for movement
     *
     * @param left Move left if true
     * @param right Move right if true
     * @param jump Jump if true (and on ground)
     */
    void handleInput(bool left, bool right, bool jump);

    // Getters
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getWidth() const { return m_width; }
    float getHeight() const { return m_height; }
    float getVelocityY() const { return m_velocityY; }
    bool isOnGround() const { return m_onGround; }

private:
    // Position and size
    float m_x;
    float m_y;
    float m_width;
    float m_height;

    // Velocity
    float m_velocityX;
    float m_velocityY;

    // Physics constants (relative to grid size)
    // Jump height: 3 grid cells
    // Move speed: 5 grid cells per second
    static constexpr float JUMP_HEIGHT_CELLS = 3.0f;
    static constexpr float MOVE_SPEED_CELLS = 5.0f;
    static constexpr float GRAVITY_MULTIPLIER = 2.5f;  // Gravity relative to jump

    // Scaled physics values
    float m_gravity;
    float m_moveSpeed;
    float m_jumpVelocity;
    float m_maxFallSpeed;

    // State
    bool m_onGround;
    bool m_jumpPressed;  // Track if jump was already pressed (prevent multi-jump)
};

// Implementation

inline Player::Player(float x, float y, float width, float height, float gridSize)
    : m_x(x)
    , m_y(y)
    , m_width(width)
    , m_height(height)
    , m_velocityX(0.0f)
    , m_velocityY(0.0f)
    , m_onGround(false)
    , m_jumpPressed(false)
{
    // Calculate physics based on grid size
    // Move speed: MOVE_SPEED_CELLS grid cells per second
    m_moveSpeed = gridSize * MOVE_SPEED_CELLS;

    // Jump velocity calculation:
    // We want to jump JUMP_HEIGHT_CELLS cells high
    // Using kinematic equation: v² = 2 * g * h
    // We'll set gravity and calculate jump velocity from desired height

    // Gravity should feel natural - scaled by grid size
    m_gravity = gridSize * MOVE_SPEED_CELLS * GRAVITY_MULTIPLIER * 2.0f;

    // Jump velocity to reach desired height
    // v = sqrt(2 * g * h)
    float jumpHeight = gridSize * JUMP_HEIGHT_CELLS;
    m_jumpVelocity = -std::sqrt(2.0f * m_gravity * jumpHeight);

    // Max fall speed: same magnitude as jump velocity * 1.5
    m_maxFallSpeed = -m_jumpVelocity * 1.5f;
}

inline void Player::update(float deltaTime, int windowHeight, float platformY) {
    // Apply gravity
    if (!m_onGround) {
        m_velocityY += m_gravity * deltaTime;

        // Clamp falling speed
        if (m_velocityY > m_maxFallSpeed) {
            m_velocityY = m_maxFallSpeed;
        }
    }

    // Update position
    m_x += m_velocityX * deltaTime;
    m_y += m_velocityY * deltaTime;

    // Platform collision
    m_onGround = false;

    if (platformY >= 0.0f) {
        // Collision with platform detected
        m_y = platformY - m_height;
        m_velocityY = 0.0f;
        m_onGround = true;
    } else {
        // Check ground collision (floor at bottom of screen)
        float groundY = static_cast<float>(windowHeight) - m_height;
        if (m_y >= groundY) {
            m_y = groundY;
            m_velocityY = 0.0f;
            m_onGround = true;
        }
    }

    // Keep player within horizontal screen bounds (simple wrap-around for now)
    // This will be improved later with proper collision
}

inline void Player::render(SDL_Renderer* renderer) const {
    // Draw player as a rectangle
    SDL_Rect rect;
    rect.x = static_cast<int>(m_x);
    rect.y = static_cast<int>(m_y);
    rect.w = static_cast<int>(m_width);
    rect.h = static_cast<int>(m_height);

    // Draw player in green
    SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Draw outline in darker green
    SDL_SetRenderDrawColor(renderer, 30, 120, 30, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

inline void Player::handleInput(bool left, bool right, bool jump) {
    // Horizontal movement
    if (left && !right) {
        m_velocityX = -m_moveSpeed;
    } else if (right && !left) {
        m_velocityX = m_moveSpeed;
    } else {
        // No input: stop moving
        m_velocityX = 0.0f;
    }

    // Jumping (only if on ground and jump just pressed)
    if (jump && m_onGround && !m_jumpPressed) {
        m_velocityY = m_jumpVelocity;
        m_onGround = false;
        m_jumpPressed = true;
    }

    // Track jump button state to prevent holding jump
    if (!jump) {
        m_jumpPressed = false;
    }
}

} // namespace entities

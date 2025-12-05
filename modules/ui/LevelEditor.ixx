module;

#include <SDL.h>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

export module ui.LevelEditor;

import ui.Button;
import ui.TextRenderer;
import core.GameState;
import world.LevelData;
import world.LevelManager;

export namespace ui {
    enum class EditorMode {
        Delete,
        PlacePlatform,
        PlaceStart,
        PlaceFinish,
        PlaceCoin
    };

    class LevelEditor {
    public:
        LevelEditor(int windowWidth, int windowHeight);

        void handleMouseMove(int x, int y);
        core::GameState handleMouseClick(int x, int y);
        void handleMouseRelease(int x, int y);
        void render(SDL_Renderer* renderer) const;

    private:
        std::unique_ptr<Button> m_backButton;
        std::unique_ptr<Button> m_saveButton;
        std::unique_ptr<Button> m_clearButton;
        std::unique_ptr<Button> m_gridSizeIncButton;
        std::unique_ptr<Button> m_gridSizeDecButton;

        std::unique_ptr<Button> m_toolDeleteButton;
        std::unique_ptr<Button> m_toolPlatformButton;
        std::unique_ptr<Button> m_toolDeadlyButton;
        std::unique_ptr<Button> m_toolUnstableButton;
        std::unique_ptr<Button> m_toolStartButton;
        std::unique_ptr<Button> m_toolFinishButton;
        std::unique_ptr<Button> m_toolCoinButton;

        std::unique_ptr<Button> m_drawerToggleButton;

        world::LevelData m_currentLevel;

        EditorMode m_mode;
        world::PlatformType m_currentPlatformType;
        bool m_isDragging;
        int m_dragStartX, m_dragStartY;
        int m_mouseX, m_mouseY;

        int m_windowWidth;
        int m_windowHeight;

        bool m_drawerOpen;

        int snapToGrid(int value) const;
        void saveLevelWithPrompt();
        int findPlatformAtPosition(int x, int y) const;
        int findCoinAtPosition(int x, int y) const;
        void updateGridSize(int newSize);
    };

    inline LevelEditor::LevelEditor(int windowWidth, int windowHeight)
        : m_currentLevel("NewLevel")
        , m_mode(EditorMode::PlacePlatform)
        , m_currentPlatformType(world::PlatformType::Normal)
        , m_isDragging(false)
        , m_dragStartX(0)
        , m_dragStartY(0)
        , m_mouseX(0)
        , m_mouseY(0)
        , m_windowWidth(windowWidth)
        , m_windowHeight(windowHeight)
        , m_drawerOpen(true) {

        m_backButton = std::make_unique<Button>(10.0f, 10.0f, 120.0f, 40.0f, "Retour");
        m_saveButton = std::make_unique<Button>(140.0f, 10.0f, 120.0f, 40.0f, "Sauver");
        m_clearButton = std::make_unique<Button>(270.0f, 10.0f, 120.0f, 40.0f, "Effacer");
        m_gridSizeDecButton = std::make_unique<Button>(400.0f, 10.0f, 40.0f, 40.0f, "-");
        m_gridSizeIncButton = std::make_unique<Button>(450.0f, 10.0f, 40.0f, 40.0f, "+");

        m_drawerToggleButton = std::make_unique<Button>(
            static_cast<float>(windowWidth) - 40.0f,
            static_cast<float>(windowHeight) / 2.0f - 30.0f,
            30.0f,
            60.0f,
            "<"
        );

        float toolButtonWidth = 180.0f;
        float toolButtonHeight = 50.0f;
        float drawerX = static_cast<float>(windowWidth) - 250.0f;
        float toolStartY = 120.0f;
        float toolSpacing = 60.0f;

        m_toolDeleteButton = std::make_unique<Button>(drawerX, toolStartY, toolButtonWidth, toolButtonHeight, "X");
        m_toolPlatformButton = std::make_unique<Button>(drawerX, toolStartY + toolSpacing, toolButtonWidth, toolButtonHeight, "P");
        m_toolDeadlyButton = std::make_unique<Button>(drawerX, toolStartY + toolSpacing * 2, toolButtonWidth, toolButtonHeight, "D");
        m_toolUnstableButton = std::make_unique<Button>(drawerX, toolStartY + toolSpacing * 3, toolButtonWidth, toolButtonHeight, "U");
        m_toolStartButton = std::make_unique<Button>(drawerX, toolStartY + toolSpacing * 4, toolButtonWidth, toolButtonHeight, "S");
        m_toolFinishButton = std::make_unique<Button>(drawerX, toolStartY + toolSpacing * 5, toolButtonWidth, toolButtonHeight, "F");
        m_toolCoinButton = std::make_unique<Button>(drawerX, toolStartY + toolSpacing * 6, toolButtonWidth, toolButtonHeight, "C");

        m_currentLevel.levelWidthCells = windowWidth / m_currentLevel.gridSize;
        m_currentLevel.levelHeightCells = windowHeight / m_currentLevel.gridSize;
    }

    inline int LevelEditor::snapToGrid(int value) const {
        return (value / m_currentLevel.gridSize) * m_currentLevel.gridSize;
    }

    inline void LevelEditor::updateGridSize(int newSize) {
        if (newSize < 8) newSize = 8;
        if (newSize > 128) newSize = 128;

        m_currentLevel.gridSize = newSize;
        m_currentLevel.levelWidthCells = m_windowWidth / newSize;
        m_currentLevel.levelHeightCells = m_windowHeight / newSize;

        std::cout << "Grid size: " << newSize << "px, Level: "
                  << m_currentLevel.levelWidthCells << "x"
                  << m_currentLevel.levelHeightCells << " cells\n";
    }

    inline int LevelEditor::findPlatformAtPosition(int x, int y) const {
        for (size_t i = 0; i < m_currentLevel.platforms.size(); ++i) {
            const auto& p = m_currentLevel.platforms[i];
            if (x >= p.x && x <= p.x + p.width &&
                y >= p.y && y <= p.y + p.height) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    inline void LevelEditor::saveLevelWithPrompt() {
        static int levelCounter = 1;
        m_currentLevel.name = "CustomLevel" + std::to_string(levelCounter++);

        world::LevelManager& manager = world::LevelManager::getInstance();
        if (manager.saveLevel(m_currentLevel)) {
            std::cout << "Level saved: " << m_currentLevel.name << "\n";
        } else {
            std::cout << "Failed to save level\n";
        }
    }

    inline void LevelEditor::handleMouseMove(int x, int y) {
        m_mouseX = x;
        m_mouseY = y;

        m_backButton->setHovered(m_backButton->contains(x, y));
        m_saveButton->setHovered(m_saveButton->contains(x, y));
        m_clearButton->setHovered(m_clearButton->contains(x, y));
        m_gridSizeDecButton->setHovered(m_gridSizeDecButton->contains(x, y));
        m_gridSizeIncButton->setHovered(m_gridSizeIncButton->contains(x, y));
        m_drawerToggleButton->setHovered(m_drawerToggleButton->contains(x, y));

        if (m_drawerOpen) {
            m_toolDeleteButton->setHovered(m_toolDeleteButton->contains(x, y));
            m_toolPlatformButton->setHovered(m_toolPlatformButton->contains(x, y));
            m_toolDeadlyButton->setHovered(m_toolDeadlyButton->contains(x, y));
            m_toolUnstableButton->setHovered(m_toolUnstableButton->contains(x, y));
            m_toolStartButton->setHovered(m_toolStartButton->contains(x, y));
            m_toolFinishButton->setHovered(m_toolFinishButton->contains(x, y));
            m_toolCoinButton->setHovered(m_toolCoinButton->contains(x, y));
        }
    }

    inline core::GameState LevelEditor::handleMouseClick(int x, int y) {
        if (m_backButton->contains(x, y)) {
            return core::GameState::MainMenu;
        }

        if (m_saveButton->contains(x, y)) {
            saveLevelWithPrompt();
            return core::GameState::LevelEditor;
        }

        if (m_clearButton->contains(x, y)) {
            m_currentLevel.clear();
            std::cout << "Level cleared\n";
            return core::GameState::LevelEditor;
        }

        if (m_drawerToggleButton->contains(x, y)) {
            m_drawerOpen = !m_drawerOpen;
            m_drawerToggleButton->setText(m_drawerOpen ? "<" : ">");
            std::cout << "Drawer " << (m_drawerOpen ? "opened" : "closed") << "\n";
            return core::GameState::LevelEditor;
        }

        if (!m_drawerOpen) {
        } else if (m_toolDeleteButton->contains(x, y)) {
            m_mode = EditorMode::Delete;
            std::cout << "Tool: Delete\n";
            return core::GameState::LevelEditor;
        } else if (m_toolPlatformButton->contains(x, y)) {
            m_mode = EditorMode::PlacePlatform;
            m_currentPlatformType = world::PlatformType::Normal;
            std::cout << "Tool: Normal Platform\n";
            return core::GameState::LevelEditor;
        } else if (m_toolDeadlyButton->contains(x, y)) {
            m_mode = EditorMode::PlacePlatform;
            m_currentPlatformType = world::PlatformType::Deadly;
            std::cout << "Tool: Lava\n";
            return core::GameState::LevelEditor;
        } else if (m_toolUnstableButton->contains(x, y)) {
            m_mode = EditorMode::PlacePlatform;
            m_currentPlatformType = world::PlatformType::Unstable;
            std::cout << "Tool: Unstable Platform\n";
            return core::GameState::LevelEditor;
        } else if (m_toolStartButton->contains(x, y)) {
            m_mode = EditorMode::PlaceStart;
            std::cout << "Tool: Start Position\n";
            return core::GameState::LevelEditor;
        } else if (m_toolFinishButton->contains(x, y)) {
            m_mode = EditorMode::PlaceFinish;
            std::cout << "Tool: Finish Position\n";
            return core::GameState::LevelEditor;
        } else if (m_toolCoinButton->contains(x, y)) {
            m_mode = EditorMode::PlaceCoin;
            std::cout << "Tool: Coin\n";
            return core::GameState::LevelEditor;
        }

        if (m_gridSizeDecButton->contains(x, y)) {
            updateGridSize(m_currentLevel.gridSize - 8);
            return core::GameState::LevelEditor;
        }

        if (m_gridSizeIncButton->contains(x, y)) {
            updateGridSize(m_currentLevel.gridSize + 8);
            return core::GameState::LevelEditor;
        }

        if (m_mode == EditorMode::PlacePlatform) {
            m_isDragging = true;
            m_dragStartX = snapToGrid(x);
            m_dragStartY = snapToGrid(y);
        } else if (m_mode == EditorMode::PlaceStart) {
            int snappedX = snapToGrid(x);
            int snappedY = snapToGrid(y);
            m_currentLevel.startX = static_cast<float>(snappedX);
            m_currentLevel.startY = static_cast<float>(snappedY);
            m_currentLevel.hasStart = true;
            std::cout << "Start position placed at (" << snappedX << ", " << snappedY << ")\n";
        } else if (m_mode == EditorMode::PlaceFinish) {
            int snappedX = snapToGrid(x);
            int snappedY = snapToGrid(y);
            m_currentLevel.finishX = static_cast<float>(snappedX);
            m_currentLevel.finishY = static_cast<float>(snappedY);
            m_currentLevel.hasFinish = true;
            std::cout << "Finish position placed at (" << snappedX << ", " << snappedY << ")\n";
        } else if (m_mode == EditorMode::PlaceCoin) {
            int snappedX = snapToGrid(x);
            int snappedY = snapToGrid(y);
            m_currentLevel.addCoin(static_cast<float>(snappedX), static_cast<float>(snappedY));
            std::cout << "Coin placed at (" << snappedX << ", " << snappedY << ")\n";
        } else if (m_mode == EditorMode::Delete) {
            int platformIndex = findPlatformAtPosition(x, y);
            if (platformIndex >= 0) {
                m_currentLevel.removePlatform(static_cast<size_t>(platformIndex));
                std::cout << "Platform deleted\n";
            }
            else if (m_currentLevel.hasStart &&
                     x >= m_currentLevel.startX && x <= m_currentLevel.startX + m_currentLevel.gridSize &&
                     y >= m_currentLevel.startY && y <= m_currentLevel.startY + m_currentLevel.gridSize) {
                m_currentLevel.hasStart = false;
                std::cout << "Start position removed\n";
            }
            else if (m_currentLevel.hasFinish &&
                     x >= m_currentLevel.finishX && x <= m_currentLevel.finishX + m_currentLevel.gridSize &&
                     y >= m_currentLevel.finishY && y <= m_currentLevel.finishY + m_currentLevel.gridSize) {
                m_currentLevel.hasFinish = false;
                std::cout << "Finish position removed\n";
            }
            else {
                int coinIndex = findCoinAtPosition(x, y);
                if (coinIndex >= 0) {
                    m_currentLevel.removeCoin(static_cast<size_t>(coinIndex));
                    std::cout << "Coin removed\n";
                }
            }
        }

        return core::GameState::LevelEditor;
    }

    inline void LevelEditor::handleMouseRelease(int x, int y) {
        if (m_isDragging && m_mode == EditorMode::PlacePlatform) {
            int endX = snapToGrid(x);
            int endY = snapToGrid(y);

            int minX = (m_dragStartX < endX) ? m_dragStartX : endX;
            int minY = (m_dragStartY < endY) ? m_dragStartY : endY;
            int width = ((m_dragStartX < endX) ? (endX - m_dragStartX) : (m_dragStartX - endX)) + m_currentLevel.gridSize;
            int height = ((m_dragStartY < endY) ? (endY - m_dragStartY) : (m_dragStartY - endY)) + m_currentLevel.gridSize;

            if (width >= m_currentLevel.gridSize && height >= m_currentLevel.gridSize) {
                m_currentLevel.addPlatform(
                    static_cast<float>(minX),
                    static_cast<float>(minY),
                    static_cast<float>(width),
                    static_cast<float>(height),
                    m_currentPlatformType
                );
                std::cout << "Platform created at (" << minX << ", " << minY
                          << ") size: " << width << "x" << height << "\n";
            }

            m_isDragging = false;
        }
    }

    inline void LevelEditor::render(SDL_Renderer* renderer) const {
        SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
        for (int x = 0; x < m_windowWidth; x += m_currentLevel.gridSize) {
            SDL_RenderDrawLine(renderer, x, 0, x, m_windowHeight);
        }
        for (int y = 0; y < m_windowHeight; y += m_currentLevel.gridSize) {
            SDL_RenderDrawLine(renderer, 0, y, m_windowWidth, y);
        }

        for (const auto& platform : m_currentLevel.platforms) {
            SDL_Rect rect = {
                static_cast<int>(platform.x),
                static_cast<int>(platform.y),
                static_cast<int>(platform.width),
                static_cast<int>(platform.height)
            };

            if (platform.type == world::PlatformType::Deadly) {
                SDL_SetRenderDrawColor(renderer, 180, 30, 30, 255);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 120, 20, 20, 255);
                SDL_RenderDrawRect(renderer, &rect);
            } else if (platform.type == world::PlatformType::Unstable) {
                SDL_SetRenderDrawColor(renderer, 50, 100, 200, 255);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 30, 60, 140, 255);
                SDL_RenderDrawRect(renderer, &rect);
            } else {
                SDL_SetRenderDrawColor(renderer, 139, 90, 43, 255);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 180, 120, 60, 255);
                SDL_RenderDrawRect(renderer, &rect);
            }
        }

        if (m_currentLevel.hasStart) {
            SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
            SDL_Rect startRect = {
                static_cast<int>(m_currentLevel.startX),
                static_cast<int>(m_currentLevel.startY),
                m_currentLevel.gridSize,
                m_currentLevel.gridSize
            };
            SDL_RenderFillRect(renderer, &startRect);
            SDL_SetRenderDrawColor(renderer, 30, 120, 30, 255);
            SDL_RenderDrawRect(renderer, &startRect);
        }

        if (m_currentLevel.hasFinish) {
            int flagX = static_cast<int>(m_currentLevel.finishX);
            int flagY = static_cast<int>(m_currentLevel.finishY);
            int cellSize = m_currentLevel.gridSize;

            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
            SDL_Rect poleRect = {
                flagX + cellSize / 2 - 2,
                flagY,
                4,
                cellSize
            };
            SDL_RenderFillRect(renderer, &poleRect);

            SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
            SDL_Rect flagRect = {
                flagX + cellSize / 2,
                flagY + cellSize / 4,
                cellSize / 2,
                cellSize / 3
            };
            SDL_RenderFillRect(renderer, &flagRect);
        }

        for (const auto& coin : m_currentLevel.coins) {
            int coinX = static_cast<int>(coin.x);
            int coinY = static_cast<int>(coin.y);
            int coinSize = static_cast<int>(m_currentLevel.gridSize * 0.6f);
            int centerX = coinX + coinSize / 2;
            int centerY = coinY + coinSize / 2;
            int radius = coinSize / 2;

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

        if (m_isDragging) {
            int endX = snapToGrid(m_mouseX);
            int endY = snapToGrid(m_mouseY);
            int minX = (m_dragStartX < endX) ? m_dragStartX : endX;
            int minY = (m_dragStartY < endY) ? m_dragStartY : endY;
            int width = ((m_dragStartX < endX) ? (endX - m_dragStartX) : (m_dragStartX - endX)) + m_currentLevel.gridSize;
            int height = ((m_dragStartY < endY) ? (endY - m_dragStartY) : (m_dragStartY - endY)) + m_currentLevel.gridSize;

            SDL_SetRenderDrawColor(renderer, 139, 90, 43, 128);
            SDL_Rect previewRect = { minX, minY, width, height };
            SDL_RenderFillRect(renderer, &previewRect);
            SDL_SetRenderDrawColor(renderer, 200, 150, 80, 255);
            SDL_RenderDrawRect(renderer, &previewRect);
        }

        m_backButton->render(renderer);
        m_saveButton->render(renderer);
        m_clearButton->render(renderer);
        m_gridSizeDecButton->render(renderer);
        m_gridSizeIncButton->render(renderer);

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_Rect infoRect = { 500, 10, 100, 40 };
        SDL_RenderFillRect(renderer, &infoRect);

        if (m_drawerOpen) {
            int drawerWidth = 280;
            int drawerX = m_windowWidth - drawerWidth;

            SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
            SDL_Rect drawerRect = { drawerX, 0, drawerWidth, m_windowHeight };
            SDL_RenderFillRect(renderer, &drawerRect);

            SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
            SDL_RenderDrawLine(renderer, drawerX, 0, drawerX, m_windowHeight);

            TextRenderer& textRenderer = TextRenderer::getInstance();
            SDL_Color titleColor = {255, 255, 255, 255};
            SDL_Texture* titleTexture = textRenderer.renderText(renderer, "Outils", 32, titleColor);
            if (titleTexture) {
                int textWidth, textHeight;
                SDL_QueryTexture(titleTexture, nullptr, nullptr, &textWidth, &textHeight);
                SDL_Rect textRect = {
                    drawerX + drawerWidth / 2 - textWidth / 2,
                    60 - textHeight / 2,
                    textWidth,
                    textHeight
                };
                SDL_RenderCopy(renderer, titleTexture, nullptr, &textRect);
                SDL_DestroyTexture(titleTexture);
            }

            TextRenderer& tr = TextRenderer::getInstance();
            SDL_Color labelColor = {230, 230, 230, 255};

            auto renderToolButton = [&](const std::unique_ptr<Button>& button, EditorMode toolMode,
                                       int r, int g, int b, const std::string& label) {
                if (m_mode == toolMode) {
                    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
                }
                SDL_Rect rect = {
                    static_cast<int>(button->getX()),
                    static_cast<int>(button->getY()),
                    static_cast<int>(button->getWidth()),
                    static_cast<int>(button->getHeight())
                };
                SDL_RenderFillRect(renderer, &rect);

                SDL_SetRenderDrawColor(renderer, 100, 100, 110, 255);
                SDL_RenderDrawRect(renderer, &rect);

                SDL_Texture* labelTexture = tr.renderText(renderer, label, 20, labelColor);
                if (labelTexture) {
                    int textWidth, textHeight;
                    SDL_QueryTexture(labelTexture, nullptr, nullptr, &textWidth, &textHeight);
                    SDL_Rect textRect = {
                        static_cast<int>(button->getX()) + static_cast<int>(button->getWidth()) / 2 - textWidth / 2,
                        static_cast<int>(button->getY()) + static_cast<int>(button->getHeight()) / 2 - textHeight / 2,
                        textWidth,
                        textHeight
                    };
                    SDL_RenderCopy(renderer, labelTexture, nullptr, &textRect);
                    SDL_DestroyTexture(labelTexture);
                }
            };

            renderToolButton(m_toolDeleteButton, EditorMode::Delete, 180, 60, 60, "Supprimer");

            if (m_mode == EditorMode::PlacePlatform && m_currentPlatformType == world::PlatformType::Normal) {
                SDL_SetRenderDrawColor(renderer, 139, 90, 43, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
            }
            SDL_Rect normalRect = {
                static_cast<int>(m_toolPlatformButton->getX()),
                static_cast<int>(m_toolPlatformButton->getY()),
                static_cast<int>(m_toolPlatformButton->getWidth()),
                static_cast<int>(m_toolPlatformButton->getHeight())
            };
            SDL_RenderFillRect(renderer, &normalRect);
            SDL_SetRenderDrawColor(renderer, 100, 100, 110, 255);
            SDL_RenderDrawRect(renderer, &normalRect);
            SDL_Texture* platformText = tr.renderText(renderer, "Plateforme", 20, labelColor);
            if (platformText) {
                int tw, th;
                SDL_QueryTexture(platformText, nullptr, nullptr, &tw, &th);
                SDL_Rect tr_rect = {
                    static_cast<int>(m_toolPlatformButton->getX()) + static_cast<int>(m_toolPlatformButton->getWidth()) / 2 - tw / 2,
                    static_cast<int>(m_toolPlatformButton->getY()) + static_cast<int>(m_toolPlatformButton->getHeight()) / 2 - th / 2,
                    tw, th
                };
                SDL_RenderCopy(renderer, platformText, nullptr, &tr_rect);
                SDL_DestroyTexture(platformText);
            }

            if (m_mode == EditorMode::PlacePlatform && m_currentPlatformType == world::PlatformType::Deadly) {
                SDL_SetRenderDrawColor(renderer, 180, 30, 30, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
            }
            SDL_Rect deadlyRect = {
                static_cast<int>(m_toolDeadlyButton->getX()),
                static_cast<int>(m_toolDeadlyButton->getY()),
                static_cast<int>(m_toolDeadlyButton->getWidth()),
                static_cast<int>(m_toolDeadlyButton->getHeight())
            };
            SDL_RenderFillRect(renderer, &deadlyRect);
            SDL_SetRenderDrawColor(renderer, 100, 100, 110, 255);
            SDL_RenderDrawRect(renderer, &deadlyRect);
            SDL_Texture* deadlyText = tr.renderText(renderer, "Lave", 20, labelColor);
            if (deadlyText) {
                int tw, th;
                SDL_QueryTexture(deadlyText, nullptr, nullptr, &tw, &th);
                SDL_Rect tr_rect = {
                    static_cast<int>(m_toolDeadlyButton->getX()) + static_cast<int>(m_toolDeadlyButton->getWidth()) / 2 - tw / 2,
                    static_cast<int>(m_toolDeadlyButton->getY()) + static_cast<int>(m_toolDeadlyButton->getHeight()) / 2 - th / 2,
                    tw, th
                };
                SDL_RenderCopy(renderer, deadlyText, nullptr, &tr_rect);
                SDL_DestroyTexture(deadlyText);
            }

            if (m_mode == EditorMode::PlacePlatform && m_currentPlatformType == world::PlatformType::Unstable) {
                SDL_SetRenderDrawColor(renderer, 50, 100, 200, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
            }
            SDL_Rect unstableRect = {
                static_cast<int>(m_toolUnstableButton->getX()),
                static_cast<int>(m_toolUnstableButton->getY()),
                static_cast<int>(m_toolUnstableButton->getWidth()),
                static_cast<int>(m_toolUnstableButton->getHeight())
            };
            SDL_RenderFillRect(renderer, &unstableRect);
            SDL_SetRenderDrawColor(renderer, 100, 100, 110, 255);
            SDL_RenderDrawRect(renderer, &unstableRect);
            SDL_Texture* unstableText = tr.renderText(renderer, "Instable", 20, labelColor);
            if (unstableText) {
                int tw, th;
                SDL_QueryTexture(unstableText, nullptr, nullptr, &tw, &th);
                SDL_Rect tr_rect = {
                    static_cast<int>(m_toolUnstableButton->getX()) + static_cast<int>(m_toolUnstableButton->getWidth()) / 2 - tw / 2,
                    static_cast<int>(m_toolUnstableButton->getY()) + static_cast<int>(m_toolUnstableButton->getHeight()) / 2 - th / 2,
                    tw, th
                };
                SDL_RenderCopy(renderer, unstableText, nullptr, &tr_rect);
                SDL_DestroyTexture(unstableText);
            }

            renderToolButton(m_toolStartButton, EditorMode::PlaceStart, 50, 200, 50, "Depart");
            renderToolButton(m_toolFinishButton, EditorMode::PlaceFinish, 220, 50, 50, "Arrivee");
            renderToolButton(m_toolCoinButton, EditorMode::PlaceCoin, 255, 215, 0, "Piece");
        }

        m_drawerToggleButton->render(renderer);
    }

    inline int LevelEditor::findCoinAtPosition(int x, int y) const {
        float coinSize = m_currentLevel.gridSize * 0.6f;
        for (size_t i = 0; i < m_currentLevel.coins.size(); ++i) {
            const auto& coin = m_currentLevel.coins[i];
            if (x >= coin.x && x <= coin.x + coinSize &&
                y >= coin.y && y <= coin.y + coinSize) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }
}

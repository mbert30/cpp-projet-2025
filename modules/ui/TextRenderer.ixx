module;

#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <string>
#include <stdexcept>

export module ui.TextRenderer;

export namespace ui {
    /**
     * @brief Singleton class for managing TTF font rendering
     *
     * This class handles SDL_ttf initialization and provides
     * text rendering capabilities with RAII principles.
     */
    class TextRenderer {
    public:
        // Singleton access
        static TextRenderer& getInstance();

        // Delete copy and move constructors
        TextRenderer(const TextRenderer&) = delete;
        TextRenderer& operator=(const TextRenderer&) = delete;
        TextRenderer(TextRenderer&&) = delete;
        TextRenderer& operator=(TextRenderer&&) = delete;

        /**
         * @brief Render text to a surface, then create a texture
         *
         * @param renderer SDL renderer
         * @param text Text to render
         * @param fontSize Font size
         * @param color Text color
         * @return Texture containing rendered text (caller must destroy)
         */
        SDL_Texture* renderText(SDL_Renderer* renderer, const std::string& text,
                                int fontSize, SDL_Color color);

        /**
         * @brief Get text dimensions without rendering
         */
        void getTextSize(const std::string& text, int fontSize, int& width, int& height);

    private:
        TextRenderer();
        ~TextRenderer();

        struct FontDeleter {
            void operator()(TTF_Font* font) const {
                if (font) TTF_CloseFont(font);
            }
        };

        TTF_Font* getOrLoadFont(int fontSize);

        std::unique_ptr<TTF_Font, FontDeleter> m_font24;
        std::unique_ptr<TTF_Font, FontDeleter> m_font18;
        bool m_initialized;
    };

    inline TextRenderer::TextRenderer()
        : m_font24(nullptr), m_font18(nullptr), m_initialized(false) {

        if (TTF_Init() < 0) {
            throw std::runtime_error("TTF initialization failed: " + std::string(TTF_GetError()));
        }
        m_initialized = true;

        // Try to load a default system font
        // On Windows, we'll use Arial which is usually available
        const char* fontPaths[] = {
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/Arial.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Linux
            "/System/Library/Fonts/Helvetica.ttc"               // macOS
        };

        TTF_Font* font24 = nullptr;
        for (const char* path : fontPaths) {
            font24 = TTF_OpenFont(path, 24);
            if (font24) {
                m_font24.reset(font24);
                break;
            }
        }

        if (!m_font24) {
            throw std::runtime_error("Failed to load any system font. Please ensure a TTF font is available.");
        }

        // Load smaller font
        TTF_Font* font18 = nullptr;
        for (const char* path : fontPaths) {
            font18 = TTF_OpenFont(path, 18);
            if (font18) {
                m_font18.reset(font18);
                break;
            }
        }
    }

    inline TextRenderer::~TextRenderer() {
        m_font24.reset();
        m_font18.reset();
        if (m_initialized) {
            TTF_Quit();
        }
    }

    inline TextRenderer& TextRenderer::getInstance() {
        static TextRenderer instance;
        return instance;
    }

    inline TTF_Font* TextRenderer::getOrLoadFont(int fontSize) {
        if (fontSize <= 18 && m_font18) {
            return m_font18.get();
        }
        return m_font24.get();
    }

    inline SDL_Texture* TextRenderer::renderText(SDL_Renderer* renderer, const std::string& text,
                                                   int fontSize, SDL_Color color) {
        if (!m_initialized || text.empty()) {
            return nullptr;
        }

        TTF_Font* font = getOrLoadFont(fontSize);
        if (!font) {
            return nullptr;
        }

        // Render text to surface
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!surface) {
            return nullptr;
        }

        // Create texture from surface
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        return texture;
    }

    inline void TextRenderer::getTextSize(const std::string& text, int fontSize, int& width, int& height) {
        if (!m_initialized || text.empty()) {
            width = 0;
            height = 0;
            return;
        }

        TTF_Font* font = getOrLoadFont(fontSize);
        if (!font) {
            width = 0;
            height = 0;
            return;
        }

        TTF_SizeText(font, text.c_str(), &width, &height);
    }
}

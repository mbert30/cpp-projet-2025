module;

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

export module world.LevelData;

export namespace world {
    /**
     * @brief Type of platform
     */
    enum class PlatformType {
        Normal,
        Deadly
    };

    /**
     * @brief Structure representing a platform in the level
     */
    struct PlatformData {
        float x, y;
        float width, height;
        PlatformType type;

        PlatformData() : x(0), y(0), width(0), height(0), type(PlatformType::Normal) {}
        PlatformData(float _x, float _y, float _w, float _h, PlatformType _type = PlatformType::Normal)
            : x(_x), y(_y), width(_w), height(_h), type(_type) {}
    };

    /**
     * @brief Structure representing a coin in the level
     */
    struct CoinData {
        float x, y;

        CoinData() : x(0), y(0) {}
        CoinData(float _x, float _y) : x(_x), y(_y) {}
    };

    /**
     * @brief Level data that can be saved/loaded to/from file
     */
    struct LevelData {
        std::string name;
        std::vector<PlatformData> platforms;
        std::vector<CoinData> coins;
        int gridSize;        
        int levelWidthCells; 
        int levelHeightCells;

        float startX, startY;
        float finishX, finishY;
        bool hasStart;
        bool hasFinish;

        LevelData()
            : name("Untitled"), gridSize(32), levelWidthCells(25), levelHeightCells(19)
            , startX(0), startY(0), finishX(0), finishY(0)
            , hasStart(false), hasFinish(false) {}
        explicit LevelData(const std::string& levelName)
            : name(levelName), gridSize(32), levelWidthCells(25), levelHeightCells(19)
            , startX(0), startY(0), finishX(0), finishY(0)
            , hasStart(false), hasFinish(false) {}

        /**
         * @brief Save level to file
         * Format: Simple text format
         * Line 1: Level name
         * Line 2+: platform x y width height
         */
        bool saveToFile(const std::string& filename) const;

        /**
         * @brief Load level from file
         */
        bool loadFromFile(const std::string& filename);

        /**
         * @brief Add a platform to the level
         */
        void addPlatform(float x, float y, float width, float height, PlatformType type = PlatformType::Normal);

        /**
         * @brief Remove platform at index
         */
        void removePlatform(size_t index);

        /**
         * @brief Clear all platforms
         */
        void clear();

        /**
         * @brief Add a coin to the level
         */
        void addCoin(float x, float y);

        /**
         * @brief Remove coin at index
         */
        void removeCoin(size_t index);
    };

    inline bool LevelData::saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        file << name << "\n";

        file << gridSize << " " << levelWidthCells << " " << levelHeightCells << "\n";

        file << hasStart << " " << startX << " " << startY << "\n";
        file << hasFinish << " " << finishX << " " << finishY << "\n";

        file << platforms.size() << "\n";

        for (const auto& platform : platforms) {
            file << platform.x << " "
                 << platform.y << " "
                 << platform.width << " "
                 << platform.height << " "
                 << static_cast<int>(platform.type) << "\n";
        }

        file << coins.size() << "\n";

        for (const auto& coin : coins) {
            file << coin.x << " " << coin.y << "\n";
        }

        file.close();
        return true;
    }

    inline bool LevelData::loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        platforms.clear();

        std::getline(file, name);

        file >> gridSize >> levelWidthCells >> levelHeightCells;

        file >> hasStart >> startX >> startY;
        file >> hasFinish >> finishX >> finishY;

        size_t platformCount;
        file >> platformCount;

        for (size_t i = 0; i < platformCount; ++i) {
            PlatformData platform;
            int type;
            file >> platform.x >> platform.y >> platform.width >> platform.height >> type;
            platform.type = static_cast<PlatformType>(type);
            platforms.push_back(platform);
        }

        size_t coinCount;
        file >> coinCount;

        for (size_t i = 0; i < coinCount; ++i) {
            CoinData coin;
            file >> coin.x >> coin.y;
            coins.push_back(coin);
        }

        file.close();
        return true;
    }

    inline void LevelData::addPlatform(float x, float y, float width, float height, PlatformType type) {
        platforms.emplace_back(x, y, width, height, type);
    }

    inline void LevelData::removePlatform(size_t index) {
        if (index < platforms.size()) {
            platforms.erase(platforms.begin() + static_cast<long>(index));
        }
    }

    inline void LevelData::clear() {
        platforms.clear();
        coins.clear();
    }

    inline void LevelData::addCoin(float x, float y) {
        coins.emplace_back(x, y);
    }

    inline void LevelData::removeCoin(size_t index) {
        if (index < coins.size()) {
            coins.erase(coins.begin() + static_cast<long>(index));
        }
    }
}

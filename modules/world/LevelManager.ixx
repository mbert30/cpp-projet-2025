module;

#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

export module world.LevelManager;

import world.LevelData;

export namespace world {
    enum class LevelCategory {
        Default,
        Custom  
    };

    /**
     * @brief Singleton class to manage all levels in the game
     */
    class LevelManager {
    public:
        static LevelManager& getInstance();

        LevelManager(const LevelManager&) = delete;
        LevelManager& operator=(const LevelManager&) = delete;
        LevelManager(LevelManager&&) = delete;
        LevelManager& operator=(LevelManager&&) = delete;

        /**
         * @brief Get level names for a specific category
         */
        std::vector<std::string> getLevelNames(LevelCategory category) const;

        /**
         * @brief Load a specific level by name and category
         */
        LevelData loadLevel(const std::string& levelName, LevelCategory category) const;

        /**
         * @brief Save a custom level (always goes to custom directory)
         */
        bool saveLevel(const LevelData& level);

        /**
         * @brief Delete a custom level
         */
        bool deleteLevel(const std::string& levelName);

        /**
         * @brief Refresh the list of available levels
         */
        void refreshLevelList();

        /**
         * @brief Get the number of levels in a category
         */
        size_t getLevelCount(LevelCategory category) const;

    private:
        LevelManager();
        ~LevelManager() = default;

        void ensureDirectoriesExist();
        std::string getLevelFilePath(const std::string& levelName, LevelCategory category) const;
        void refreshCategory(LevelCategory category);

        std::string m_defaultLevelsDir;
        std::string m_customLevelsDir;
        std::vector<std::string> m_defaultLevelNames;
        std::vector<std::string> m_customLevelNames;
    };

    inline LevelManager::LevelManager()
        : m_defaultLevelsDir("levels/default/")
        , m_customLevelsDir("levels/custom/") {
        ensureDirectoriesExist();
        refreshLevelList();
    }

    inline LevelManager& LevelManager::getInstance() {
        static LevelManager instance;
        return instance;
    }

    inline void LevelManager::ensureDirectoriesExist() {
        std::filesystem::create_directories(m_defaultLevelsDir);
        std::filesystem::create_directories(m_customLevelsDir);
    }

    inline std::string LevelManager::getLevelFilePath(const std::string& levelName, LevelCategory category) const {
        if (category == LevelCategory::Default) {
            return m_defaultLevelsDir + levelName + ".lvl";
        } else {
            return m_customLevelsDir + levelName + ".lvl";
        }
    }

    inline void LevelManager::refreshCategory(LevelCategory category) {
        std::vector<std::string>& levelNames = (category == LevelCategory::Default)
            ? m_defaultLevelNames
            : m_customLevelNames;

        std::string directory = (category == LevelCategory::Default)
            ? m_defaultLevelsDir
            : m_customLevelsDir;

        levelNames.clear();

        if (!std::filesystem::exists(directory)) {
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".lvl") {
                levelNames.push_back(entry.path().stem().string());
            }
        }

        // Sort alphabetically
        std::sort(levelNames.begin(), levelNames.end());
    }

    inline void LevelManager::refreshLevelList() {
        refreshCategory(LevelCategory::Default);
        refreshCategory(LevelCategory::Custom);
    }

    inline std::vector<std::string> LevelManager::getLevelNames(LevelCategory category) const {
        return (category == LevelCategory::Default)
            ? m_defaultLevelNames
            : m_customLevelNames;
    }

    inline size_t LevelManager::getLevelCount(LevelCategory category) const {
        return (category == LevelCategory::Default)
            ? m_defaultLevelNames.size()
            : m_customLevelNames.size();
    }

    inline LevelData LevelManager::loadLevel(const std::string& levelName, LevelCategory category) const {
        LevelData level;
        std::string filepath = getLevelFilePath(levelName, category);
        level.loadFromFile(filepath);
        return level;
    }

    inline bool LevelManager::saveLevel(const LevelData& level) {
        std::string filepath = m_customLevelsDir + level.name + ".lvl";
        bool success = level.saveToFile(filepath);
        if (success) {
            const_cast<LevelManager*>(this)->refreshCategory(LevelCategory::Custom);
        }
        return success;
    }

    inline bool LevelManager::deleteLevel(const std::string& levelName) {
        std::string filepath = m_customLevelsDir + levelName + ".lvl";
        bool success = std::filesystem::remove(filepath);
        if (success) {
            refreshCategory(LevelCategory::Custom);
        }
        return success;
    }
}

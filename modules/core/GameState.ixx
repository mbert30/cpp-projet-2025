module;

export module core.GameState;

export namespace core {
    enum class GameState {
        MainMenu,
        LevelSelect,
        Playing,
        LevelEditor,
        Options,
        Victory,
        Quit
    };
}

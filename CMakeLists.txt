# Configuration de CMake pour Platformer Game - C++ Moderne avec Modules
cmake_minimum_required(VERSION 3.29)

# Nom du projet et de l'exécutable généré
project(PlatformerGame VERSION 1.0.0 LANGUAGES CXX)

# CRITICAL: Enable experimental C++23 module support
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.28")
    set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "3c375311-a3c9-4396-a187-3227ef642046")
    set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
endif()

# Configuration du type de build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# Forcer l'utilisation de C++ 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Tout doit être parfait
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
# Ne pas utiliser les extensions de CMake
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF)
endif()

# Répertoire contenant les sources
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
# Répertoire contenant les modules
set(MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules)
# Répertoire contenant les tests unitaires
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Compiler warnings (équivalent de compile_options.cmake)
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Find SDL2 - configuration manuelle pour MinGW
set(SDL2_ROOT "C:/SDL2/x86_64-w64-mingw32")
set(SDL2_INCLUDE_DIR "${SDL2_ROOT}/include/SDL2")
set(SDL2_LIBRARY "${SDL2_ROOT}/lib/libSDL2.dll.a")
set(SDL2MAIN_LIBRARY "${SDL2_ROOT}/lib/libSDL2main.a")

# Find SDL2_ttf - configuration manuelle pour MinGW
set(SDL2_TTF_ROOT "C:/SDL2_ttf/x86_64-w64-mingw32")
set(SDL2_TTF_INCLUDE_DIR "${SDL2_TTF_ROOT}/include/SDL2")
set(SDL2_TTF_LIBRARY "${SDL2_TTF_ROOT}/lib/libSDL2_ttf.dll.a")

# Include SDL2 and SDL2_ttf headers (needed for global module fragment in .ixx files)
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})

#######################################################
# Partie à mettre à jour manuellement
# Exécutable principal avec support des modules
add_executable(${PROJECT_NAME}
    ${SRC_DIR}/main.cpp
)

# Define module sources using FILE_SET for CMake module scanning
target_sources(${PROJECT_NAME}
    PUBLIC
        FILE_SET CXX_MODULES FILES
            ${MODULE_DIR}/core/GameState.ixx
            ${MODULE_DIR}/core/Game.ixx
            ${MODULE_DIR}/entities/Player.ixx
            ${MODULE_DIR}/entities/Coin.ixx
            ${MODULE_DIR}/world/Platform.ixx
            ${MODULE_DIR}/world/LevelData.ixx
            ${MODULE_DIR}/world/LevelManager.ixx
            ${MODULE_DIR}/world/Level.ixx
            ${MODULE_DIR}/ui/TextRenderer.ixx
            ${MODULE_DIR}/ui/Button.ixx
            ${MODULE_DIR}/ui/MainMenu.ixx
            ${MODULE_DIR}/ui/LevelSelectMenu.ixx
            ${MODULE_DIR}/ui/LevelEditor.ixx
            ${MODULE_DIR}/ui/OptionsMenu.ixx
            ${MODULE_DIR}/ui/VictoryScreen.ixx
)

# Link SDL2 and SDL2_ttf libraries manuellement
target_link_libraries(${PROJECT_NAME}
    ${SDL2MAIN_LIBRARY}
    ${SDL2_LIBRARY}
    ${SDL2_TTF_LIBRARY}
    mingw32
    m
    dinput8
    dxguid
    dxerr8
    user32
    gdi32
    winmm
    imm32
    ole32
    oleaut32
    shell32
    setupapi
    version
    uuid
)

#######################################################

# Copy assets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Copy SDL2.dll and SDL2_ttf.dll to the executable directory for runtime
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_ROOT}/bin/SDL2.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_TTF_ROOT}/bin/SDL2_ttf.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying SDL2.dll and SDL2_ttf.dll to executable directory"
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

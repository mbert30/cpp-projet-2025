# Platformer Game - C++ Modern

Jeu de plateforme 2D développé en C++20 moderne avec SFML.

## Prérequis

- **CMake** >= 3.20
- **C++20** compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- **SFML** >= 2.5

### Installation de SFML

#### Windows (avec vcpkg recommandé)
```bash
vcpkg install sfml
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install libsfml-dev
```

#### macOS (avec Homebrew)
```bash
brew install sfml
```

## Structure du projet

```
cpp-projet-2025/
├── CMakeLists.txt          # Configuration CMake
├── src/
│   ├── main.cpp            # Point d'entrée
│   ├── core/               # Cœur du moteur (Game loop, Window)
│   ├── entities/           # Entités du jeu (Player, Enemies, Platforms)
│   ├── physics/            # Physique (Collisions, Gravité)
│   ├── graphics/           # Rendu (Sprites, Animations)
│   └── utils/              # Utilitaires
├── include/                # Fichiers d'en-tête
├── assets/                 # Ressources (textures, sons, maps)
└── tests/                  # Tests unitaires
```

## Compilation

### Méthode 1 : CMake classique

```bash
# Créer le dossier de build
mkdir build
cd build

# Configurer le projet
cmake ..

# Compiler
cmake --build .

# Exécuter
./bin/PlatformerGame       # Linux/macOS
.\bin\PlatformerGame.exe   # Windows
```

### Méthode 2 : Avec vcpkg (Windows recommandé)

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

## Contrôles actuels

- **ESC** : Quitter le jeu
- **X (fermeture fenêtre)** : Quitter le jeu

## Fonctionnalités implémentées

- [x] Structure de base du projet
- [x] Game loop avec delta time
- [x] Fenêtre SFML avec gestion des événements
- [x] Architecture moderne C++20
- [ ] Système de physique
- [ ] Joueur avec contrôles
- [ ] Plateformes et collisions
- [ ] Ennemis
- [ ] Système d'animation
- [ ] Gestion des niveaux
- [ ] Menu principal
- [ ] Sons et musique

## Principes C++ Moderne utilisés

- **Smart pointers** : `std::unique_ptr` pour la gestion automatique de la mémoire
- **RAII** : Acquisition des ressources dans le constructeur
- **C++20 Standard** : `constexpr`, `[[maybe_unused]]`, `= default`, `= delete`
- **Move semantics** : Constructeurs et opérateurs de déplacement
- **Namespaces** : Organisation logique du code
- **Modern CMake** : Target-based configuration

## Auteur

Projet dans le cadre du TP C++ Moderne - Master 2

## License

Projet éducatif

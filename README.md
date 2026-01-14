# Outpost Zero
## Game details
- This game is a simplistic sci-fi tower defense game. 
- Enemies spawn at one point, and travel a pre-defined path towards the end. 
- Once the enemies cross the entire path, they deplete your health, 1 per enemy.

## Description

Outpost Zero is a 2D tower defense game built with C++ and raylib. Players must strategically deploy and upgrade a variety of turrets along a fixed path to defend their base against waves of incoming enemies. The game features multiple enemy types, distinct turret upgrade paths, and periodic boss waves that increase the game's difficulty and reward players for their strategic prowess.

## Features

*   **Classic Tower Defense Gameplay:** Easy to learn, challenging to master.
*   **Diverse Turret Arsenal:** 8 unique turrets across 3 categories (Projectile, Laser, and Status Effect), each with multiple upgrade tiers.
*   **Varied Enemy Roster:** Multiple enemy types with unique stats and abilities, from fast-moving swarms to durable, self-healing tanks.
*   **Dynamic Difficulty:** The game is divided into stages, with enemy health permanently increasing after each boss wave to keep the late game challenging.
*   **Boss Enemies:** Face off against powerful boss enemies in the occassional wave.
*   **Raylib:** Built from the ground up in C++17 using raylib library.
*   **Immediate-Mode GUI:** All in-game UI is handled by the `raygui` library.

## How to Build and Run

These instructions are for Linux users. 

### Prerequisites

*   A C++17 compliant compiler (like g++)
*   GNU Make
*   `git`

### Instructions

1.  Clone the repository and its submodules recursively:
    ```bash
    git clone --recursive https://github.com/Man1ac-1773/OutpostZero_1101.git
    ```
    *If you have already cloned the repository without the `--recursive` flag, navigate into the project directory and run:*
    ```bash
    git submodule update --init --recursive
    ```

2.  Navigate into the project directory:
    ```bash
    cd OutpostZero_1101
    ```

3.  Build the project using the provided Makefile:
    ```bash
    make
    ```

4.  Run the game:
    ```bash
    ./build/output
    ```
    Alternatively, you can use the provided shell script to build and run in one step:
    ```bash
    ./run.sh
    ```

## Gameplay Overview

The core objective is to prevent enemies from reaching the end of a pre-defined path. Each enemy that gets through depletes your base health by one point.

*   **Turrets:** You can build three base types of turrets, which can then be upgraded along two distinct paths, resulting in 8 unique turrets in total.
    *   **Projectile Turrets:** Your standard damage dealers, evolving from single-shot cannons to powerful shotguns and flamethrowers.
    *   **Laser Turrets:** High-tech weaponry that fires piercing or high-damage beams.
    *   **Status Effect Turrets:** Utility towers that can slow enemies, making them vulnerable to other defenses.

*   **Enemies:** You will face a variety of enemies, each requiring different strategies to defeat.
*   **Economy:** Defeating enemies earns you currency, which you can use to build new turrets or upgrade existing ones. Managing your economy is key to victory.
*   For more information on how to play, refer `TUTORIAL.md`.


## Code Architecture
The project is structured to be modular and scalable, with a clear separation of concerns.

*   **Scene Management (`main.cpp`):** The main entry point handles the primary game loop and switches between different game scenes (e.g., Intro, Game, Settings). The core gameplay logic is driven by the `Game()` function in `src/scenes/Game.cpp`.
*   **Entity System:**
    *   All game objects (Turrets, Enemies, Projectiles) inherit from a base `Entity` class.
    *   These entities are managed in a central `std::vector<std::unique_ptr<Entity>>` in the `Game` scene, simplifying the update and render loops. Smart pointers are used for automatic memory management.
    *   Virtual functions like `Update()` and `Draw()` are overridden by each subclass, allowing for polymorphic behavior in the main loop.
*   **Modular Design:**
    *   **`Turret.h` / `Enemy.h` / `Projectile.h`:** Each major game component has its own header file defining its class structure and inheritance hierarchy.
    *   **`Map.h`:** Handles the generation, storage, and drawing of the game map, including pathing information for enemies.
    *   **`Wave.h`:** The `WaveManager` class is responsible for orchestrating the sequence and timing of enemy spawns for each wave.
    *   **`Particle.h`:** A self-contained particle system adds visual flair to explosions and other effects.
    *   **`utils.h`:** A collection of helper functions (e.g., for math and drawing). For simplicity and to reduce the number of source files, these are provided as `inline` functions in a single header.
*   **GUI (`raygui.h`):** All UI elements, such as buttons and info panels, are drawn at the end of the frame using the immediate-mode `raygui` library.

## Credits

*   The textures for the turrets and some enemies are inspired by and adapted from the open-source game **Mindustry**.
*   Some UI textures were generated with assistance from **Google Gemini**.

#include "Config.h"
#include <raylib.h>
//initial screen dimensions
    int screenWidth = 1000;
    int screenHeight = 600;

//intro
    const int introBoxWidth = 250;
    const int introBoxHeight = 75;
    const int introBoxSpacing = 25;
    const int introMaxWidth = 275;
    const int introMaxHeight = 100;
    const int introZoomSpeed = 4;

// colors
    const Color pathColor = Color{ 100, 100, 100, 255 };
    const Color buildableColor = Color{ 80, 80, 80, 255 }; 
    const Color gridLineColor = Color{ 60, 60, 60, 100 }; // colour of border of each tile

// targets
    std::vector<Vector2> targets;
    Vector2 startPos; 

// Tile system
    const int GRID_ROWS = 14; 
    const int GRID_COLS = 21; 
    const int TILE_SIZE = 40;

// constants for enemies
    // for class standard_enemy
    const float standard_enemy_radius = 5.0f;
    const Color standard_enemy_color = RED; 
    const float standard_enemy_speed = 100.0f; 

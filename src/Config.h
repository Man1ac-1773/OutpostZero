#pragma once
#include <raylib.h>
#include <vector>
//initial screen dimensions
    extern int screenWidth;
    extern int screenHeight;

//intro
    extern const int introBoxWidth;
    extern const int introBoxHeight;
    extern const int introBoxSpacing;
    extern const int introMaxWidth;
    extern const int introMaxHeight;
    extern const int introZoomSpeed;
// game

// colors
    extern const Color pathColor;
    extern const Color buildableColor; 
    extern const Color gridLineColor;
    extern const Color mouseHoverColor; 

// path followed by enemy
    extern std::vector<Vector2> targets; 
    extern Vector2 startPos; 

// map constants  
    extern const int TILE_SIZE;  // Each tile is NxN pixels 
    extern const int GRID_COLS;  
    extern const int GRID_ROWS;

// constants for enemies
    // for standard_enemy
    extern const float standard_enemy_radius;
    extern const Color standard_enemy_color; 
    extern const float standard_enemy_speed; 
    extern const float standard_enemy_health;

    // for fast_enemy
    extern const float fast_enemy_radius; 
    extern const float fast_enemy_speed; 
    extern const Color fast_enemy_color; 
    extern const float fast_enemy_health; 

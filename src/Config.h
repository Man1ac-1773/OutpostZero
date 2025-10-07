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

// path followed by enemy
    extern std::vector<Vector2> targets; 
    extern Vector2 startPos; 

// Tile system constants
    extern const int TILE_SIZE;  // Each tile is NxN pixels 
    extern const int GRID_COLS;  // Number of columns 
    extern const int GRID_ROWS;  // Number of rows 


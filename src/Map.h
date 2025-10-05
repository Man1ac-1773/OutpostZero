#pragma once
#include "raylib.h"
#include <vector>
#include "Config.h"
class Entity; 

enum class TileType{
    PATH, 
    BUILDABLE,
    OTHER
};

struct Tile{
    Rectangle rect; 
    TileType type = TileType::BUILDABLE;
    bool hasTurret = false;
    Color color; 
};

class Map{
public: 
    // Tile system constants
    static const int TILE_SIZE = 40;  // Each tile is NxN pixels 
    static const int GRID_COLS = 21;  // Number of columns 
    static const int GRID_ROWS = 14;  // Number of rows 
                               //
    std::vector<std::vector<Tile>> grid; // 2D grid of tiles
    const Color pathColor = Color{ 100, 100, 100, 255 };
    const Color buildableColor = Color{ 80, 80, 80, 255 }; 
    const Color gridLineColor = Color{ 60, 60, 60, 100 }; // colour of border of each tile
    
    Map(); 

    Tile* getTileFromMouse(Vector2 pos); 
    void Draw(); 

};

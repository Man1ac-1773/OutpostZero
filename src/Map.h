#pragma once
#include "Config.h"
#include "raylib.h"
#include <vector>
class Entity;

enum class TileType
{
    PATH,
    BUILDABLE,
    OTHER
};

struct Tile
{
    Rectangle rect;
    TileType type = TileType::BUILDABLE;
    bool hasTurret = false;
    Color color;
};

class Map
{
  public:
    std::vector<std::vector<Tile>> grid; // 2D grid of tiles
    const Color pathColor = Color{100, 100, 100, 255};
    const Color buildableColor = Color{80, 80, 80, 255};
    const Color gridLineColor = Color{60, 60, 60, 100}; // colour of border of each tile

    Map();

    Tile *getTileFromMouse(Vector2 pos);
    void Draw();
};

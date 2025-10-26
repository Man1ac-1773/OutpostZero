#include "Map.h"
#include "Config.h"
#include <raylib.h>

Map::Map()
{
    grid.clear();
    targets.clear();
    grid.resize(GRID_ROWS);
    // create and allocate memory
    for (int row = 0; row < GRID_ROWS; row++)
    {
        grid[row].resize(GRID_COLS);
        for (int col = 0; col < GRID_COLS; col++)
        {
            grid[row][col].rect = {(float)(col * TILE_SIZE), (float)(row * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE};
        }
    }
    int pathCol = 1;
    bool goingDown = true;
    int segmentSpacing = 3;

    /// loop to assign walking path as TileType::PATH
    while (pathCol < GRID_COLS)
    {
        int row;
        int col = 0;
        if (goingDown)
        {
            // going down
            for (row = 1; row < GRID_ROWS - 1; row++)
            {
                grid[row][pathCol].type = TileType::PATH;
            }
            row--;
            targets.push_back({grid[row][pathCol].rect.x + (float)TILE_SIZE / 2, grid[row][pathCol].rect.y + (float)TILE_SIZE / 2});
            // going right
            if (pathCol + segmentSpacing < GRID_COLS)
            {
                for (col = 0; col < segmentSpacing; col++)
                {
                    grid[GRID_ROWS - 2][pathCol + col].type = TileType::PATH;
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            // going up
            for (row = GRID_ROWS - 2; row >= 1; row--)
            {
                grid[row][pathCol].type = TileType::PATH;
            }
            row++;
            targets.push_back({grid[row][pathCol].rect.x + (float)TILE_SIZE / 2, grid[row][pathCol].rect.y + (float)TILE_SIZE / 2});

            // going right
            if (pathCol + segmentSpacing < GRID_COLS)
            {

                for (col = 0; col < segmentSpacing; col++)
                {
                    grid[1][pathCol + col].type = TileType::PATH;
                }
            }
            else
            {
                break;
            }
        }

        pathCol += segmentSpacing;

        // add the right segment motion after either down or up loop is finished
        targets.push_back({grid[row][pathCol].rect.x + (float)TILE_SIZE / 2, grid[row][pathCol + col].rect.y + (float)TILE_SIZE / 2});

        goingDown = !goingDown;
    }

    // entry-exit tile
    grid[0][1].type = TileType::PATH;
    grid[GRID_ROWS - 1][GRID_COLS - 2].type = TileType::PATH;
    startPos = Vector2{grid[0][1].rect.x + (float)TILE_SIZE / 2, grid[0][1].rect.y + (float)TILE_SIZE / 2};
    // add final exit target to the targets vector
    targets.push_back({grid[GRID_ROWS - 1][GRID_COLS - 2].rect.x + (float)TILE_SIZE / 2, grid[GRID_ROWS - 1][GRID_COLS - 2].rect.y + (float)TILE_SIZE / 2});
}

void Map::Draw()
{
    for (int r = 0; r < GRID_ROWS; r++)
    {
        for (int c = 0; c < GRID_COLS; c++)
        {
            Tile &tile = grid[r][c];
            if (tile.type == TileType::PATH)
            {
                DrawRectangleRec(tile.rect, pathColor);
            }
            else if (tile.type == TileType::BUILDABLE)
            {
                DrawRectangleRec(tile.rect, buildableColor);
            }
            // grid line with boundary draw
            DrawRectangleLinesEx(tile.rect, 1, gridLineColor);

            // highlight on hover
            if (tile.type == TileType::BUILDABLE && !tile.hasTurret)
            {
                if (CheckCollisionPointRec(GetMousePosition(), tile.rect))
                {
                    DrawRectangleRec(tile.rect, mouseHoverColor);
                }
            }
        }
    }
}

Tile *Map::getTileFromMouse(Vector2 pos)
{
    int col = pos.x > GRID_COLS * TILE_SIZE ? -1 : pos.x / TILE_SIZE;
    int row = pos.y > GRID_ROWS * TILE_SIZE ? -1 : pos.y / TILE_SIZE;
    if (row > -1 && col > -1)
    {
        return &grid[row][col];
    }
    return nullptr;
}

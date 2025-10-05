#include "Map.h"
#include "Config.h"
#include <raylib.h>


Map::Map(){
    grid.clear(); 
    int totalWidth = GRID_COLS*TILE_SIZE; 
    int totalHeight = GRID_ROWS*TILE_SIZE; 

    grid.resize(GRID_ROWS); 
    // create and allocate memory 
    for (int row = 0; row < GRID_ROWS; row++){
        grid[row].resize(GRID_COLS); 
        for (int col = 0; col < GRID_COLS; col++){
            grid[row][col].rect = {
                (float)(col*TILE_SIZE),
                (float)(row*TILE_SIZE),
                (float)TILE_SIZE,
                (float)TILE_SIZE
            }; 
        }
    }
    // Entry point top-left, but one
    int pathCol = 1;
    
    bool goingDown = true;
    int segmentSpacing = 3; // Tiles between vertical path segments
    
    /// loop to assign walking path as TileType::PATH
    while (pathCol < GRID_COLS ) {
        if (goingDown) {
            // going down 
            for (int row = 1; row < GRID_ROWS-1; row++) {
                grid[row][pathCol].type = TileType::PATH;
            }
            
            // going right
            if (pathCol + segmentSpacing < GRID_COLS){
                for (int col = pathCol; col < pathCol + segmentSpacing; col++) {
                    grid[GRID_ROWS - 2][col].type = TileType::PATH;
                }
            }
        } 
        else {
            // going down 
            for (int row = 1; row < GRID_ROWS-1; row++) {
                grid[row][pathCol].type = TileType::PATH;
            }
            
            // going right
            if (pathCol + segmentSpacing < GRID_COLS) {
                for (int col = pathCol; col < pathCol + segmentSpacing; col++) {
                    grid[1][col].type = TileType::PATH;
                }
            }
        }
        
        pathCol += segmentSpacing;
        goingDown = !goingDown;
    }
    
    // entry-exit tile 
    grid[0][1].type = TileType::PATH;
    grid[GRID_ROWS-1][GRID_COLS-2].type = TileType::PATH; 

}   

void Map::Draw(){
    for (int r = 0; r < GRID_ROWS; r++){
        for (int c = 0; c < GRID_COLS; c++){
            Tile& tile = grid[r][c]; 
            if (tile.type == TileType::PATH){
                DrawRectangleRec(tile.rect, pathColor); 
            }
            else if (tile.type == TileType::BUILDABLE){
                DrawRectangleRec(tile.rect,buildableColor); 
            }
            // grid line with boundary draw
            DrawRectangleLinesEx(tile.rect, 1, gridLineColor);

            // highlight on hover
            if (tile.type == TileType::BUILDABLE && !tile.hasTurret){
                if (CheckCollisionPointRec(GetMousePosition(), tile.rect)){
                    DrawRectangleRec(tile.rect, Color{100,255,100,50});
                }
            }
        }
        
    }
}

Tile* Map::getTileFromMouse(Vector2 pos){
    int t = pos.x/TILE_SIZE; 
    int row = t>(GRID_COLS*TILE_SIZE)?-1:t; 
    int c = pos.y/TILE_SIZE; 
    int col = t > (GRID_ROWS*TILE_SIZE) ? -1 :c; 
    if (row > -1 && col > -1){
        return &grid[row][col]; 
    }
    else return nullptr; 
    

}


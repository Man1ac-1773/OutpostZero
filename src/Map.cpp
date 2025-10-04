#include "Map.h"


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

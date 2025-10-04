#include "Map.h"


Map::Map(){
    grid.clear(); 
    int totalWidth = GRID_COLS*TILE_SIZE; 
    int totalHeight = GRID_ROWS*TILE_SIZE; 

    grid.resize(GRID_ROWS); 
    /// Create and initialise the entire map to buildable
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
    // Entry point: top-left, one tile in from the edge
    int entryCol = 1;
    int pathCol = entryCol;
    
    // Create zigzag path pattern
    bool goingDown = true;
    int segmentSpacing = 3; // Number of tiles between vertical path segments
    
    while (pathCol < GRID_COLS - 1) {
        if (goingDown) {
            // DOWN segment (from top to bottom)
            for (int row = 0; row < GRID_ROWS; row++) {
                grid[row][pathCol].type = TileType::PATH;
            }
            
            // Horizontal connection at BOTTOM (going right)
            if (pathCol + segmentSpacing < GRID_COLS){
                for (int col = pathCol; col < pathCol + segmentSpacing; col++) {
                    grid[GRID_ROWS - 1][col].type = TileType::PATH;
                }
            }
        } 
        else {
            // UP segment (from bottom to top)
            for (int row = 0; row < GRID_ROWS; row++) {
                grid[row][pathCol].type = TileType::PATH;
            }
            
            // Horizontal connection at TOP (going right)
            if (pathCol + segmentSpacing < GRID_COLS) {
                for (int col = pathCol; col < pathCol + segmentSpacing; col++) {
                    grid[0][col].type = TileType::PATH;
                }
            }
        }
        
        pathCol += segmentSpacing;
        goingDown = !goingDown;
    }
    
    // Mark entry tile (top-left corner where enemy enters)
    grid[0][entryCol].type = TileType::PATH;

}    

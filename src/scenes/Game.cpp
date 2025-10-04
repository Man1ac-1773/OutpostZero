// Game.cpp
/// Global includes
#include <vector>
#include <algorithm>
#include <memory> 
/// Custom includes
#include "raylib.h"
#include "scenes.h"
#include "Config.h" 
#include "Entity.h"
#include "Turret.h"
#include "Enemy.h"
#include "Projectile.h"
using namespace std; 

// Tile system constants
const int TILE_SIZE = 40;  // Each tile is 40x40 pixels 
const int GRID_COLS = 21; // Number of columns 
const int GRID_ROWS = 14;  // Number of rows 

// Tile types
enum TileType {
    PATH,       // Enemy path
    BUILDABLE   // Can place turrets here
};

struct Tile {
    Rectangle rect;
    TileType type;
    bool hasTurret;
};

static vector<vector<Tile>> grid; // 2D grid of tiles
static vector<Rectangle> walls;   // Visual walls (for drawing)

static Color green = Color{ 57, 255, 20, 255 };


/** 
 * @brief Initialize the tile grid and path
 * @param None
 * @note CALL ONLY ON STARTUP
 */
void GenerateMap() {
    grid.clear();
    walls.clear();
    
    // Calculate offset to center the grid
    int totalWidth = GRID_COLS * TILE_SIZE;
    int totalHeight = GRID_ROWS * TILE_SIZE;
    int offsetX = (screenWidth - totalWidth) / 2;
    int offsetY = (screenHeight - totalHeight) / 2;
    
    // Initialize all tiles as buildable first
    grid.resize(GRID_ROWS);
    for (int row = 0; row < GRID_ROWS; row++) {
        grid[row].resize(GRID_COLS);
        for (int col = 0; col < GRID_COLS; col++) {
            Tile& tile = grid[row][col];
            tile.rect = {
                (float)(offsetX + col * TILE_SIZE),
                (float)(offsetY + row * TILE_SIZE),
                (float)TILE_SIZE,
                (float)TILE_SIZE
            };
            tile.type = BUILDABLE;
            tile.hasTurret = false;
        }
    }
    
    // Define the zigzag path: DOWN -> RIGHT -> UP -> RIGHT -> DOWN -> ...
    // Path is now 1 tile wide
    int pathWidth = 1;
    
    // Entry point: top-left, one tile in from the edge
    int entryCol = 1;
    int pathCol = entryCol;
    
    // Create zigzag path pattern
    bool goingDown = true;
    int segmentSpacing = 3; // Number of tiles between vertical path segments
    
    while (pathCol < GRID_COLS ) {
        if (goingDown) {
            // DOWN segment (from top to bottom)
            for (int row = 1; row < GRID_ROWS-1; row++) {
                grid[row][pathCol].type = PATH;
            }
            
            // Horizontal connection at BOTTOM (going right)
            if (pathCol + segmentSpacing < GRID_COLS) {
                for (int col = pathCol; col < pathCol + segmentSpacing; col++) {
                    grid[GRID_ROWS - 2][col].type = PATH;
                }
            }
        } else {
            // UP segment (from bottom to top)
            for (int row = 1; row < GRID_ROWS-1; row++) {
                grid[row][pathCol].type = PATH;
            }
            
            // Horizontal connection at TOP (going right)
            if (pathCol + segmentSpacing < GRID_COLS) {
                for (int col = pathCol; col < pathCol + segmentSpacing; col++) {
                    grid[1][col].type = PATH;
                }
            }
        }
        
        pathCol += segmentSpacing;
        goingDown = !goingDown;
    }
    
    // Mark entry tile (top-left corner where enemy enters)
    grid[0][entryCol].type = PATH;
    grid[GRID_ROWS-1][GRID_COLS-2].type = PATH; 

    
    // Generate visual walls on all buildable tiles adjacent to path
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            if (grid[row][col].type == BUILDABLE) {
                Rectangle wallRect = grid[row][col].rect;
                bool leftIsPath = (col > 0 && grid[row][col-1].type == PATH);
                bool rightIsPath = (col < GRID_COLS-1 && grid[row][col+1].type == PATH);
                bool topIsPath = (row > 0 && grid[row-1][col].type == PATH);
                bool bottomIsPath = (row < GRID_ROWS-1 && grid[row+1][col].type == PATH);
                
                // Only create walls if adjacent to path
                if (leftIsPath || rightIsPath || topIsPath || bottomIsPath) {
                    if (leftIsPath) {
                        walls.push_back({wallRect.x, wallRect.y, 3, wallRect.height});
                    }
                    if (rightIsPath) {
                        walls.push_back({wallRect.x + wallRect.width - 3, wallRect.y, 3, wallRect.height});
                    }
                    if (topIsPath) {
                        walls.push_back({wallRect.x, wallRect.y, wallRect.width, 3});
                    }
                    if (bottomIsPath) {
                        walls.push_back({wallRect.x, wallRect.y + wallRect.height - 3, wallRect.width, 3});
                    }
                }
            }
        }
    }
}

// Get tile at mouse position
Tile* GetTileAtPosition(Vector2 pos) {
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            if (CheckCollisionPointRec(pos, grid[row][col].rect)) {
                return &grid[row][col];
            }
        }
    }
    return nullptr;
}

static const float cameraSpeed = 500.0f;
static bool initialized = false;
static Camera2D camera = {0}; 
Vector2 screenCenter = {(float)screenWidth/2, (float)screenHeight/2};

static vector<unique_ptr<Entity>> entities; // Use a vector to hold all our entities

Scene Game(){
    
    if (!initialized){
        camera.target = screenCenter; 
        camera.offset = screenCenter; 
        camera.rotation = 0.0f; 
        camera.zoom = 1.0f; 
        GenerateMap(); // Generate the tile-based map
        initialized = true;
    }

    /// ---- INPUT PASS ----
    
    // Spawn enemy at mouse 
    if (IsKeyDown(KEY_X)) {
        Vector2 spawnPoint;
        float enemySpeed = 100.0f;
        spawnPoint = GetScreenToWorld2D(GetMousePosition(), camera);
        entities.push_back(
        make_unique<Enemy>(spawnPoint, screenCenter, enemySpeed, 5.0f, RED));
    }
    
    // Spawn turret at mouse (only on buildable tiles)
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        Vector2 mousePos = GetMousePosition();
        Tile* tile = GetTileAtPosition(mousePos);
        
        if (tile && tile->type == BUILDABLE && !tile->hasTurret) {
            // Place turret at center of tile
            Vector2 turretPos = {
                tile->rect.x + tile->rect.width / 2,
                tile->rect.y + tile->rect.height / 2
            };
            entities.push_back(
                make_unique<Turret>(turretPos, TILE_SIZE * 0.4f, 200.0f, BLUE));
            tile->hasTurret = true;
        }
    }

    // ------ UPDATE PASS ------
    // Calling simple Update for all entities
    for (auto& entity : entities) {
        entity->Update(GetFrameTime());
    }

    // ---- INTERACTION PASS -----
    // Separate entities into turrets, enemies, and projectiles
    vector<Turret*> turret_ptrs;
    vector<Enemy*> enemy_ptrs;
    vector<Projectile*> projectile_ptrs;
    vector<unique_ptr<Entity>> newProjectiles;
    
    for (auto& entity : entities) {
        if (!entity->IsActive())
            continue;
        if (Turret* t = dynamic_cast<Turret*>(entity.get())) {
            turret_ptrs.push_back(t);
        } else if (Enemy* e = dynamic_cast<Enemy*>(entity.get())) {
            enemy_ptrs.push_back(e);
        } else if (Projectile* p = dynamic_cast<Projectile*>(entity.get())) {
            projectile_ptrs.push_back(p);
        }
    }
    
    // Update turrets with knowledge of enemies and projectiles
    for (auto& turret : turret_ptrs) {
        turret->Update(GetFrameTime(), enemy_ptrs, newProjectiles);
    }
    
    // Update enemy with new player location
    for (auto& enemy : enemy_ptrs) {
        enemy->Update(screenCenter);
    }

    // Projectiles interact with enemies
    for (auto* projectile : projectile_ptrs) {
        if (Vector2DistanceSqr(projectile->GetPosition(), projectile->start_pos) > 1 << 20) {
            projectile->Destroy();
            continue;
        }
        for (auto* enemy : enemy_ptrs) {
            if (!enemy->IsActive() || !projectile->IsActive())
                continue;
            if (CheckCollisionCircles(projectile->GetPosition(),
                                        projectile->GetRadius(),
                                        enemy->GetPosition(),
                                        enemy->GetRadius())) {
                projectile->Destroy();
                enemy->Destroy();
                break;
            }
        }
    }
    
    // --- CLEANUP AND ADDITION PASS ---
    for (auto& p : newProjectiles) {
        entities.push_back(std::move(p));
    }

    entities.erase(
        remove_if(entities.begin(),
                entities.end(),
                [](const auto& entity) { return !entity->IsActive(); }),
        entities.end());

    // ---- DRAWING ----
    ClearBackground(RAYWHITE);
    BeginMode2D(camera);
    
        // Draw tiles
        for (int row = 0; row < GRID_ROWS; row++) {
            for (int col = 0; col < GRID_COLS; col++) {
                Tile& tile = grid[row][col];
                
                if (tile.type == PATH) {
                    DrawRectangleRec(tile.rect, pathColor);
                } else if (tile.type == BUILDABLE) {
                    DrawRectangleRec(tile.rect, buildableColor);
                }
                
                // Draw grid lines
                DrawRectangleLinesEx(tile.rect, 1, gridLineColor);
                
                // Highlight buildable tiles on hover
                if (tile.type == BUILDABLE && !tile.hasTurret) {
                    if (CheckCollisionPointRec(GetMousePosition(), tile.rect)) {
                        DrawRectangleRec(tile.rect, Color{100, 255, 100, 50});
                    }
                }
            }
        }
        
        // Draw border walls (solid perimeter except at entry and exit)
        float borderX = grid[0][0].rect.x;
        float borderY = grid[0][0].rect.y;
        float borderWidth = GRID_COLS * TILE_SIZE;
        float borderHeight = GRID_ROWS * TILE_SIZE;
        float wallThickness = 8.0f;
        
        // Top border (with gap at entry - column 1)
        DrawRectangle(borderX, borderY - wallThickness, grid[0][1].rect.x - borderX, wallThickness, green);
        DrawRectangle(grid[0][1].rect.x + TILE_SIZE, borderY - wallThickness, 
                      borderX + borderWidth - (grid[0][1].rect.x + TILE_SIZE), wallThickness, green);
        
        // Bottom border (check for exit and leave gap)
        int exitCol = -1;
        for (int col = GRID_COLS - 1; col >= 0; col--) {
            if (grid[GRID_ROWS - 1][col].type == PATH) {
                exitCol = col;
                break;
            }
        }
        if (exitCol >= 0) {
            DrawRectangle(borderX, borderY + borderHeight, grid[0][exitCol].rect.x - borderX, wallThickness, green);
            DrawRectangle(grid[0][exitCol].rect.x + TILE_SIZE, borderY + borderHeight,
                          borderX + borderWidth - (grid[0][exitCol].rect.x + TILE_SIZE), wallThickness, green);
        } else {
            DrawRectangle(borderX, borderY + borderHeight, borderWidth, wallThickness, green);
        }
        
        // Left border (solid)
        DrawRectangle(borderX - wallThickness, borderY, wallThickness, borderHeight, green);
        
        // Right border (solid)
        DrawRectangle(borderX + borderWidth, borderY, wallThickness, borderHeight, green);
        
        // Draw walls
        for (auto& wall : walls){
            DrawRectangleRec(wall, green);
        }
        
        // Draw entities
        for (auto& entity : entities) {
            entity->Draw();
        }
        
    EndMode2D();
    
    DrawFPS(screenWidth-80, 10);
    DrawText("Left Click: Place Turret | X: Spawn Enemy", 10, 10, 20, BLACK);

    return Scene::GAME;
}

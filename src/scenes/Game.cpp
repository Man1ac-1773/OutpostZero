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
#include "Map.h"
using namespace std; 

static vector<Rectangle> walls;   // Visual walls (for drawing)
static Map gameMap; 
static Color green = Color{ 57, 255, 20, 255 };

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
        initialized = true;
    }

    /// ---- INPUT PASS ----
    
    // Spawn enemy at mouse 
    if (IsKeyDown(KEY_X)) {
        float enemySpeed = 100.0f;
        entities.push_back(
        make_unique<Enemy>(startPos, enemySpeed, 5.0f, RED));
    }
    
    // Spawn turret at mouse (only on buildable tiles)
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        Vector2 mousePos = GetMousePosition();  
        Tile* tile = gameMap.getTileFromMouse(mousePos);
        
        if (tile != nullptr && tile->type == TileType::BUILDABLE && !tile->hasTurret) {
            // Place turret at center of tile
            Vector2 turretPos = {
                tile->rect.x + tile->rect.width / 2,
                tile->rect.y + tile->rect.height / 2
            };
            entities.push_back(
                make_unique<Turret>(turretPos, gameMap.TILE_SIZE * 0.4f, 200.0f, BLUE));
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
        enemy->Update();
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
    
        // Draw map
        gameMap.Draw(); 
        // Draw entities
        for (auto& entity : entities) {
            entity->Draw();
        }
        
    EndMode2D();
    
    DrawFPS(screenWidth-80, 10);
    DrawText("Left Click: Place Turret | X: Spawn Enemy", 10, 10, 20, BLACK);
    
    return Scene::GAME;
}

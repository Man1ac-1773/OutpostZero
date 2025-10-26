// Game.cpp
// standard includes
#include <algorithm>
#include <memory>
#include <vector>
// custom includes
#include "Config.h"
#include "Enemy.h"
#include "Entity.h"
#include "Map.h"
#include "Projectile.h"
#include "Turret.h"
#include "raygui.h"
#include "raylib.h"
#include "scenes.h"
using namespace std;
enum class buildState
{
    NONE,
    BASIC,
    LASER,
    SLOWING,
};
static Map gameMap;
static Color green = Color{57, 255, 20, 255}; // for walls, if required

static const float cameraSpeed = 500.0f; // if camera movement implemented
static bool initialized = false;
static Camera2D camera = {0};
static buildState current_build;
Vector2 screenCenter = {(float)screenWidth / 2, (float)screenHeight / 2};

static vector<unique_ptr<Entity>> entities; // Use a vector to hold all our entities

Scene Game()
{

    if (!initialized)
    {
        camera.target = screenCenter;
        camera.offset = screenCenter;
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;
        initialized = true;
        current_build = buildState::NONE;
        Turret::LoadTextures();
        Projectile::LoadTextures();
        Enemy::LoadTextures();
    }

    // ---- INPUT PASS ----
    if (IsKeyPressed(KEY_ESCAPE))
    {
        current_build = buildState::NONE;
    }
    // Spawn enemy at mouse
    if (IsKeyPressed(KEY_X))
    {
        entities.push_back(make_unique<standard_enemy>());
    }
    if (IsKeyPressed(KEY_Z))
    {
        entities.push_back(make_unique<fast_enemy>());
    }

    // Spawn turret at mouse (only on buildable tiles)
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        Tile *tile = gameMap.getTileFromMouse(mousePos);

        if (tile != nullptr && tile->type == TileType::BUILDABLE && !tile->hasTurret && current_build != buildState::NONE)
        {
            // Place turret at center of tile
            Vector2 turretPos = {tile->rect.x + tile->rect.width / 2, tile->rect.y + tile->rect.height / 2};
            switch (current_build)
            {
            case buildState::BASIC:
            {
                entities.push_back(make_unique<basic_turret>(turretPos, *tile));
                break;
            }
            case buildState::LASER:
            {
                entities.push_back(make_unique<laser_turret>(turretPos, *tile));
                break;
            }
            case buildState::SLOWING:
            {
                entities.push_back(make_unique<slowing_turret>(turretPos, *tile));
                break;
            }
            }
            tile->hasTurret = true;
        }
    }

    // ------ UPDATE PASS ------
    // Calling simple Update for all entities
    for (auto &entity : entities)
    {
        entity->Update(GetFrameTime());
    }
    particles.Update(GetFrameTime());

    // ---- INTERACTION PASS -----
    // Separate entities into turrets, enemies, and projectiles
    vector<Turret *> turret_ptrs;
    vector<Enemy *> enemy_ptrs;
    vector<Projectile *> projectile_ptrs;
    vector<unique_ptr<Entity>> newProjectiles;

    for (auto &entity : entities)
    {
        if (!entity->IsActive())
            continue;
        if (Turret *t = dynamic_cast<Turret *>(entity.get()))
        {
            turret_ptrs.push_back(t);
        }
        else if (Enemy *e = dynamic_cast<Enemy *>(entity.get()))
        {
            enemy_ptrs.push_back(e);
        }
        else if (Projectile *p = dynamic_cast<Projectile *>(entity.get()))
        {
            projectile_ptrs.push_back(p);
        }
    }

    // Update turrets with knowledge of enemies
    for (auto &turret : turret_ptrs)
    {
        turret->Update(GetFrameTime(), enemy_ptrs, newProjectiles);
    }

    // Update enemy
    for (auto &enemy : enemy_ptrs)
    {
        enemy->Update();
    }

    // Projectiles interact with enemies
    for (auto *projectile : projectile_ptrs)
    {
        // checking each projectile with each enemy is highly inefficient, but I don't know how to optimise this yet
        for (auto *enemy : enemy_ptrs)
        {
            if (!enemy->IsActive() || !projectile->IsActive())
                continue;
            if (CheckCollisionCircles(projectile->GetPosition(), projectile->GetRadius(), enemy->GetPosition(), enemy->GetRadius()))
            {
                projectile->Destroy();
                enemy->TakeDamage(projectile->getProjType());
                // enemy->DamageAnimation();
            }
        }
    }

    // --- CLEANUP AND ADDITION PASS ---
    for (auto &p : newProjectiles)
    {
        entities.push_back(std::move(p));
    }

    entities.erase(remove_if(entities.begin(), entities.end(), [](const auto &entity) { return !entity->IsActive(); }), entities.end());

    // ---- DRAWING ----
    ClearBackground(RAYWHITE);
    BeginMode2D(camera);

    // Draw map
    gameMap.Draw();
    // Draw entities
    for (auto &entity : entities)
    {
        entity->Draw();
    }
    // draw range of turrets if mouseHover
    for (auto &turret : turret_ptrs)
    {
        turret->drawRangeOnHover(GetMousePosition());
    }
    // draw range if current_build is a turret
    if (current_build != buildState::NONE)
    {
        switch (current_build)
        {
        case buildState::BASIC:
        {
            DrawCircleLinesV(GetMousePosition(), duo_turret_range, YELLOW);
            break;
        }
        case buildState::LASER:
        {
            DrawCircleLinesV(GetMousePosition(), cyclone_turret_range, YELLOW);
            break;
        }
        case buildState::SLOWING:
        {
            DrawCircleLinesV(GetMousePosition(), slowing_turret_range, BLUE);
            break;
        }
        }
    }
    particles.Draw();

    EndMode2D();
    // ----- DRAW GUI PART -----
    Rectangle basic_turret_buttonRect = {0, screenHeight - TILE_SIZE, TILE_SIZE, TILE_SIZE};
    Rectangle laser_turret_buttonRect = {TILE_SIZE, screenHeight - TILE_SIZE, TILE_SIZE, TILE_SIZE};
    Rectangle slow_turret_buttonRect = {2 * TILE_SIZE, screenHeight - TILE_SIZE, TILE_SIZE, TILE_SIZE};

    if (GuiButton(basic_turret_buttonRect, ""))
    {
        current_build = buildState::BASIC;
    }
    if (GuiButton(laser_turret_buttonRect, ""))
    {
        current_build = buildState::LASER;
    }
    if (GuiButton(slow_turret_buttonRect, ""))
    {
        current_build = buildState::SLOWING;
    }
    DrawTexturePro(Turret::basicTurretGunTexture, {0, 0, (float)(Turret::basicTurretGunTexture.width), (float)(Turret::basicTurretGunTexture.height)}, {(float)Turret::basicTurretGunTexture.width / 2.0f, (screenHeight - (float)Turret::basicTurretGunTexture.height) + 6.0f, TILE_SIZE, TILE_SIZE}, {Turret::basicTurretGunTexture.width / 2.0f, Turret::basicTurretGunTexture.height / 2.0f}, 0.0f, WHITE);

    DrawTexturePro(Turret::laserTurretGunTexture, {0, 0, (float)(Turret::laserTurretGunTexture.width), (float)(Turret::laserTurretGunTexture.height)}, {TILE_SIZE + (float)Turret::laserTurretGunTexture.width / 2.0f, (screenHeight - (float)Turret::laserTurretGunTexture.height) + 27.0f, TILE_SIZE, TILE_SIZE}, {Turret::laserTurretGunTexture.width / 2.0f, Turret::laserTurretGunTexture.height / 2.0f}, 0.0f, WHITE);
    DrawTexturePro(Turret::slowTurretTX, {0, 0, (float)(Turret::slowTurretTX.width), (float)(Turret::slowTurretTX.height)}, {2 * TILE_SIZE + (float)Turret::slowTurretTX.width / 2.0f, (screenHeight - (float)Turret::slowTurretTX.height) + 20.0f, TILE_SIZE, TILE_SIZE}, {Turret::slowTurretTX.width / 2.0f, Turret::slowTurretTX.height / 2.0f}, 0.0f, WHITE);

    DrawFPS(screenWidth - 80, 10);
    DrawText("Z: Spawn Fast enemy | X: Spawn Standard Enemy", 10, 10, 20, BLACK);
    DrawText(TextFormat("Enemies : %d", Enemy::enemy_count), screenWidth - MeasureText("Enemies : xxx", 20), 30, 20, BLACK);
    DrawText(TextFormat("Health : %d", player_health), screenWidth - MeasureText("Health : x    ", 20), screenHeight - 28, 20, RED);
    if (player_health <= 0)
    {
        DrawText("GAME OVER !! ", screenWidth / 2.0f - 300, screenHeight / 2.0f - 30, 100, RED);
        for_each(enemy_ptrs.begin(), enemy_ptrs.end(), [](Enemy *e) { e->Destroy(); });
    }
    DrawTexturePro(Enemy::heartTX, {0, 0, (float)Enemy::heartTX.width, (float)Enemy::heartTX.height}, {(float)screenWidth - 20.0f, (float)screenHeight - 20.0f, (float)Enemy::heartTX.width, (float)Enemy::heartTX.height}, {Enemy::heartTX.width / 2.0f, Enemy::heartTX.height / 2.0f}, 0.0f, WHITE);
    return Scene::GAME;
}
/* I understand recognise the use of magic numbers and the potential harms that come with it.
 * I am, in this case, and under this time-constraint, left with no better choice.
 * I might come back to improve it later
 */

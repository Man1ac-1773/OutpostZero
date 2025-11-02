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
    DUO,
    LANCER,
    WAVE,
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
        entities.push_back(make_unique<flare_enemy>());
    }
    if (IsKeyPressed(KEY_Z))
    {
        entities.push_back(make_unique<mono_enemy>());
    }
    if (IsKeyPressed(KEY_C))
    {
        entities.push_back(make_unique<crawler_enemy>());
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
            case buildState::DUO:
            {
                entities.push_back(make_unique<smite_turret>(turretPos, *tile));
                break;
            }
            case buildState::LANCER:
            {
                entities.push_back(make_unique<lancer_turret>(turretPos, *tile));
                break;
            }
            case buildState::WAVE:
            {
                entities.push_back(make_unique<salvo_turret>(turretPos, *tile));
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

    // ---- INTERACTION PASS -----
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
                projectile->ReducePierceCount();
                enemy->TakeDamage(projectile->getProjType(), GetDamageFalloff(1.0f, 0.0f, projectile->enemies_hit));
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
        case buildState::DUO:
        {
            DrawCircleLinesV(GetMousePosition(), duo_turret_range, YELLOW);
            break;
        }
        case buildState::LANCER:
        {
            DrawCircleLinesV(GetMousePosition(), lancer_turret_range, YELLOW);
            break;
        }
        case buildState::WAVE:
        {
            DrawCircleLinesV(GetMousePosition(), wave_turret_range, BLUE);
            break;
        }
        }
    }
    particles.Draw();

    EndMode2D();
    // ----- DRAW GUI -----
    Rectangle basic_turret_buttonRect = {0, screenHeight - TILE_SIZE, TILE_SIZE, TILE_SIZE};
    Rectangle laser_turret_buttonRect = {TILE_SIZE, screenHeight - TILE_SIZE, TILE_SIZE, TILE_SIZE};
    Rectangle slow_turret_buttonRect = {2 * TILE_SIZE, screenHeight - TILE_SIZE, TILE_SIZE, TILE_SIZE};

    if (GuiButton(basic_turret_buttonRect, ""))
    {
        current_build = buildState::DUO;
    }
    if (GuiButton(laser_turret_buttonRect, ""))
    {
        current_build = buildState::LANCER;
    }
    if (GuiButton(slow_turret_buttonRect, ""))
    {
        current_build = buildState::WAVE;
    }
    DrawTexturePro(Turret::duoTurretTexture, {0, 0, (float)(Turret::duoTurretTexture.width), (float)(Turret::duoTurretTexture.height)}, {(float)Turret::duoTurretTexture.width / 2.0f, (screenHeight - (float)Turret::duoTurretTexture.height) + 6.0f, TILE_SIZE, TILE_SIZE}, {Turret::duoTurretTexture.width / 2.0f, Turret::duoTurretTexture.height / 2.0f}, 0.0f, WHITE);

    DrawTexturePro(Turret::lancerTurretTexture, {0, 0, (float)(Turret::lancerTurretTexture.width), (float)(Turret::lancerTurretTexture.height)}, {TILE_SIZE + (float)Turret::lancerTurretTexture.width / 2.0f, (screenHeight - (float)Turret::lancerTurretTexture.height) + 20.0f, TILE_SIZE, TILE_SIZE}, {Turret::lancerTurretTexture.width / 2.0f, Turret::lancerTurretTexture.height / 2.0f}, 0.0f, WHITE);
    DrawTexturePro(Turret::waveTurretTX, {0, 0, (float)(Turret::waveTurretTX.width), (float)(Turret::waveTurretTX.height)}, {2 * TILE_SIZE + (float)Turret::waveTurretTX.width / 2.0f, (screenHeight - (float)Turret::waveTurretTX.height) + 20.0f, TILE_SIZE, TILE_SIZE}, {Turret::waveTurretTX.width / 2.0f, Turret::waveTurretTX.height / 2.0f}, 0.0f, WHITE);

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
/* I recognise the use of magic numbers and the potential harms that come with it.
 * I am, in this case, and under this time-constraint, left with no better choice.
 * I might come back to improve it later
 */

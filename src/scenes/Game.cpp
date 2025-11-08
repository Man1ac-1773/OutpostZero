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
#include "Wave.h"
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

static buildState current_build;
static Map gameMap;
static bool initialized = false;
static Camera2D camera = {0};
static vector<unique_ptr<Entity>> entities; // Use a vector to hold all our entities
static WaveManager wave_manager;
static Turret *currentTurret = nullptr;

Scene Game()
{
    if (!initialized)
    {
        Vector2 screenCenter = {(float)screenWidth / 2, (float)screenHeight / 2};
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
    if (IsKeyPressed(KEY_A))
    {
        entities.push_back(make_unique<poly_enemy>());
    }
    if (IsKeyPressed(KEY_S))
    {
        entities.push_back(make_unique<locus_enemy>());
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
                {   // Use the global constant for build cost check
                    if (playerMoney >= duo_turret_cost)
                    {
                        entities.push_back(make_unique<duo_turret>(turretPos, tile));
                        tile->hasTurret = true;
                        playerMoney -= duo_turret_cost; // This was correct, but I'm including it for completeness
                    }
                    break;
                }
                case buildState::LANCER:
                {
                    if (playerMoney >= lancer_turret_cost)
                    {
                        entities.push_back(make_unique<lancer_turret>(turretPos, tile));
                        tile->hasTurret = true;
                        playerMoney -= lancer_turret_cost; // This was correct, but I'm including it for completeness
                    }
                    break;
                }
                case buildState::WAVE:
                {
                    if (playerMoney >= wave_turret_cost)
                    {   
                        entities.push_back(make_unique<wave_turret>(turretPos, tile));
                        tile->hasTurret = true;
                        playerMoney -= wave_turret_cost; // This was correct, but I'm including it for completeness
                    }
                    break;
                }
            }
            
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
        enemy->DoEnemyAction(enemy_ptrs, GetFrameTime());
        enemy->Update();
    }
    // update wave information
    wave_manager.Update(GetFrameTime(), entities, enemy_ptrs.size());

    // ---- INTERACTION PASS -----
    // Projectiles interact with enemies
    for (auto *projectile : projectile_ptrs)
    {
        // checking each projectile with each enemy is highly inefficient, but I don't know how to optimise this yet
        for (auto *enemy : enemy_ptrs)
        {
            if (!enemy->IsActive() || !projectile->IsActive())
                continue;
            /* for collision checking
             * we are basically checking if this projectile
             * has the enemy_id in it's "currently colliding" stack,
             * So as to prevent cases where collisions are detected each frame,
             * before the projectile has had a chance to leave the hitbox of
             * enemy.
             * If they are colliding => check if they have already collided
             * else => remove from current_colliding stack;
             */
            bool colliding = CheckCollisionCircles(projectile->GetPosition(), projectile->GetRadius(), enemy->GetPosition(), enemy->GetRadius());
            if (colliding)
            {
                // New collision this frame
                if (projectile->current_colliding.find(enemy->id) == projectile->current_colliding.end())
                {
                    projectile->current_colliding.insert(enemy->id);
                    projectile->ReducePierceCount();
                    enemy->TakeDamage(projectile->getProjType(), GetDamageFalloff(1.0f, 0.0f, projectile->enemies_hit));
                }
            }
            else
            {
                // has collided
                projectile->current_colliding.erase(enemy->id);
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
    Rectangle nextWaveButton = {GRID_COLS * TILE_SIZE + 30, TILE_SIZE, 2 * TILE_SIZE, 2 * TILE_SIZE};
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
    if (GuiButton(nextWaveButton, "Next wave"))
    {
        if (wave_manager.CanStartNextWave())
        {
            wave_manager.StartNextWave();
        }
    }
    /* This block is here and not merged with the input pass because
    * I wanted to loop through turrets vectors after they have been populated
    * in the update pass
    */
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mousePos = GetMousePosition();
        Tile *tile = gameMap.getTileFromMouse(mousePos);
        if (tile != nullptr && tile->hasTurret && current_build == buildState::NONE)
        {
            for (auto &turret : turret_ptrs)
            {
                if (CheckCollisionPointCircle(mousePos, turret->GetPosition(), turret->radius))
                {
                    currentTurret = turret;
                }
            }
        }
    }

    // ---- three turret draw calls ----
    // These draw the textures on the buttons
    DrawTexturePro(Turret::duoTurretTexture, {0, 0, (float)(Turret::duoTurretTexture.width), (float)(Turret::duoTurretTexture.height)}, {(float)Turret::duoTurretTexture.width / 2.0f, (screenHeight - (float)Turret::duoTurretTexture.height) + 6.0f, TILE_SIZE, TILE_SIZE}, {Turret::duoTurretTexture.width / 2.0f, Turret::duoTurretTexture.height / 2.0f}, 0.0f, WHITE);

    DrawTexturePro(Turret::lancerTurretTexture, {0, 0, (float)(Turret::lancerTurretTexture.width), (float)(Turret::lancerTurretTexture.height)}, {TILE_SIZE + (float)Turret::lancerTurretTexture.width / 2.0f, (screenHeight - (float)Turret::lancerTurretTexture.height) + 20.0f, TILE_SIZE, TILE_SIZE}, {Turret::lancerTurretTexture.width / 2.0f, Turret::lancerTurretTexture.height / 2.0f}, 0.0f, WHITE);

    DrawTexturePro(Turret::waveTurretTexture, {0, 0, (float)(Turret::waveTurretTexture.width), (float)(Turret::waveTurretTexture.height)}, {2 * TILE_SIZE + (float)Turret::waveTurretTexture.width / 2.0f, (screenHeight - (float)Turret::waveTurretTexture.height) + 20.0f, TILE_SIZE, TILE_SIZE}, {Turret::waveTurretTexture.width / 2.0f, Turret::waveTurretTexture.height / 2.0f}, 0.0f, WHITE);
    // ---- ----

    // --- Drawing heart and currency textures ---
    DrawTexturePro(Enemy::heartTX, {0, 0, (float)Enemy::heartTX.width, (float)Enemy::heartTX.height}, {(float)screenWidth - 20.0f, (float)screenHeight - 20.0f, (float)Enemy::heartTX.width, (float)Enemy::heartTX.height}, {Enemy::heartTX.width / 2.0f, Enemy::heartTX.height / 2.0f}, 0.0f, WHITE);
    DrawTexture(Enemy::currencyTX, screenWidth - 100, 40, WHITE);
    // ---- ----

    // turret info draw call
    if (currentTurret) {
        if (currentTurret->DrawTurretInfo(entities))
            currentTurret = nullptr;
    } else {
        // If no turret is selected, show build info for the turret being placed
        switch (current_build)
        {
            case buildState::DUO:
                duo_turret::DrawBuildInfo();
                break;
            case buildState::LANCER:
                lancer_turret::DrawBuildInfo();
                break;
            case buildState::WAVE:
                wave_turret::DrawBuildInfo();
                break;
            case buildState::NONE:
                // Do nothing
                break;
        }
    }

    // -----
    DrawFPS(screenWidth - 80, 10);
    DrawText(TextFormat("Health : %d", player_health), screenWidth - MeasureText("Health : x      ", 20), screenHeight - 28, 20, RED);
    DrawText(TextFormat(" : %d", playerMoney), screenWidth - 80, 40, 20, GREEN);
    if (player_health <= 0)
    {
        DrawText("GAME OVER !! ", screenWidth / 2.0f - 300, screenHeight / 2.0f - 30, 100, RED);
        for_each(enemy_ptrs.begin(), enemy_ptrs.end(), [](Enemy *e) { e->Destroy(); });
    }
    // Draw Wave Counter
    DrawText(TextFormat("Wave: %d / %d", wave_manager.GetWaveNumber(), wave_manager.GetTotalWaves()), GRID_COLS * TILE_SIZE + 30, 80, 20, BLACK);

    return Scene::GAME;
}
/* I recognise the use of magic numbers and the potential harms that come with it.
 * I am, in this case, and under this time-constraint, left with no better choice.
 * I might come back to improve it later
 */

#pragma once
#include "Enemy.h"
#include "Entity.h"
#include "Map.h"
#include "Particles.h"
#include "Projectile.h"
#include "Types.h"
#include "raygui.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include <Config.h>
#include <iostream>
#include <memory> // for std::unique_ptr
#include <vector>

/* Turret.h declaration of all turrets in the game
 * Using classes and inheritance to handle multiple objects
 * using virtual functions which are overriden so that the main loop calls the correct function for each object
 */

class Turret : public Entity
{
  public:
    // base properties
    float radius;
    float range;
    Color color;
    // shooting properties
    float cooldownTimer;
    float fireTimer;

    // gun properties
    float gunRotation; // degrees
    float rotationSpeed;
    float recoilOffset;
    float m_recoilOffset;
    Tile *tileOfTurret;
    TurretType turret;

    Turret(Vector2 pos, Tile *tile, float speed, TurretType t)
    {
        position = pos;     // position of the turret
        gunRotation = 0.0f; // initial rotation of the gun (for visual aiming)
        tileOfTurret = tile;
        projectileSpeed = speed;
        radius = 20.0f;
        turret = t;
    }

    void Update(float deltaTime) override
    {
        // primitive update function
        if (fireTimer > 0)
        {
            fireTimer -= deltaTime;
            recoilOffset = Lerp(recoilOffset, 0, 0.2f);
        }
    }
    virtual void Update(float deltaTime, const std::vector<Enemy *> &targets, std::vector<std::unique_ptr<Entity>> &newProjectiles)
    {
        Enemy *target = nullptr;
        float closestDist = 999999.0f;

        for (auto &e_ptr : targets)
        {
            if (!e_ptr->isVisible && turret != TurretType::SALVO)
                continue;
            if (turret == TurretType::SALVO && e_ptr->status_effect == StatusEffects::SLOWED)
                continue;
            float dist = Vector2Distance(position, e_ptr->GetPosition());
            if (dist < range && dist < closestDist)
            {
                closestDist = dist;
                target = e_ptr;
            }
        }
        if (target != nullptr)
        {

            Vector2 aimPoint = CalculateInterceptPoint(target->GetPosition(), target->GetVelocity(), projectileSpeed);

            float targetAngle = atan2f(aimPoint.y - position.y, aimPoint.x - position.x) * RAD2DEG;

            float rotationSpeed = GetRotationSpeed();
            gunRotation = MoveAngle(gunRotation, targetAngle, rotationSpeed);
            float angleDifference = normaliseAngle(targetAngle - gunRotation);
            if (fireTimer <= 0 && angleDifference < 8.0f)
            {

                switch (turret)
                {
                case TurretType::DUO:
                {
                    newProjectiles.push_back(std::make_unique<normal_bullet>(position, aimPoint));
                    break;
                }
                case TurretType::SMITE:
                {
                    int num_proj = 10;
                    // fire n projectiles in a certain cone
                    float spread = 30.0f * DEG2RAD; // degrees
                    float central_angle = atan2f(aimPoint.y - position.y, aimPoint.x - position.x);
                    for (int i = 0; i < num_proj; i++)
                    {
                        float offset = -spread / 2 + i * (spread / (num_proj - 1));
                        float shot_angle = central_angle + offset;
                        Vector2 dir = {cosf(shot_angle), sinf(shot_angle)};
                        Vector2 aim_point = Vector2Add(position, Vector2Scale(dir, smite_turret_range));
                        newProjectiles.push_back(std::make_unique<shotgun_bullet>(position, aim_point));
                    }
                    break;
                }
                case TurretType::LANCER:
                {
                    newProjectiles.push_back(std::make_unique<laser_bullet>(position, aimPoint));
                    break;
                }
                case TurretType::RIPPLE:
                {
                    newProjectiles.push_back(std::make_unique<flame_bullet>(position, aimPoint));
                    break;
                }
                case TurretType::SALVO:
                {
                    newProjectiles.push_back(std::make_unique<ice_bullet>(position, aimPoint));
                    break;
                }
                }

                fireTimer = cooldownTimer;
                recoilOffset = m_recoilOffset;
            }
        }
    }
    virtual void Draw() override = 0;
    virtual bool DrawTurretInfo(vector<unique_ptr<Entity>> &entities) = 0;
    // name is self-explanatory
    void drawRangeOnHover(Vector2 pos)
    {
        if (CheckCollisionPointRec(pos, tileOfTurret->rect))
        {
            DrawCircleLines(position.x, position.y, range, YELLOW);
        }
    }

    // static functions
    // independent of instance of class
    // Can be called independently
    // which i am doing in Game.cpp
    static void LoadTextures()
    {
        // loading base 1
        turretBaseIMG = LoadImage("assets/turrets/base-1.png");
        turretBaseTexture = LoadTextureFromImage(turretBaseIMG);
        UnloadImage(turretBaseIMG);
        // base 2 for T-3 turrets
        turretBaseIMG_2 = LoadImage("assets/turrets/base-2.png");
        ImageResize(&turretBaseIMG_2, TILE_SIZE, TILE_SIZE);
        turretBaseTexture_2 = LoadTextureFromImage(turretBaseIMG_2);
        UnloadImage(turretBaseIMG_2);
        // --- PROJECTILE TURRETS ---
        // duo turret
        duoTurretTexture = LoadTexture("assets/turrets/duo.png");

        // ripple turret
        rippleTurretIMG = LoadImage("assets/turrets/ripple.png");
        ImageResize(&rippleTurretIMG, TILE_SIZE, TILE_SIZE);
        rippleTurretTexture = LoadTextureFromImage(rippleTurretIMG);
        UnloadImage(rippleTurretIMG);

        // smite turret
        smiteTurretIMG = LoadImage("assets/turrets/smite.png");
        ImageResize(&smiteTurretIMG, TILE_SIZE + 5.0f, TILE_SIZE + 5.0f);
        smiteTurretTexture = LoadTextureFromImage(smiteTurretIMG);
        UnloadImage(smiteTurretIMG);

        // --- LASER TURRETS ---
        lancerTurretIMG = LoadImage("assets/turrets/lancer.png");
        ImageResize(&lancerTurretIMG, TILE_SIZE, TILE_SIZE);
        lancerTurretTexture = LoadTextureFromImage(lancerTurretIMG);
        UnloadImage(lancerTurretIMG);

        cycloneTurretIMG = LoadImage("assets/turrets/cyclone.png");
        ImageResize(&cycloneTurretIMG, TILE_SIZE, TILE_SIZE);
        cycloneTurretTexture = LoadTextureFromImage(cycloneTurretIMG);
        UnloadImage(cycloneTurretIMG);

        meltdownTurretIMG = LoadImage("assets/turrets/meltdown.png");
        ImageResize(&meltdownTurretIMG, TILE_SIZE + 5.0f, TILE_SIZE + 5.0f);
        meltdownTurretTexture = LoadTextureFromImage(meltdownTurretIMG);
        UnloadImage(meltdownTurretIMG);
        // --- ---

        // --- STATUS TURRETS ----
        waveTurretIMG = LoadImage("assets/turrets/wave.png");
        ImageResize(&waveTurretIMG, TILE_SIZE, TILE_SIZE);
        waveTurretTexture = LoadTextureFromImage(waveTurretIMG);
        UnloadImage(waveTurretIMG);

        salvoTurretIMG = LoadImage("assets/turrets/salvo.png");
        ImageResize(&salvoTurretIMG, TILE_SIZE, TILE_SIZE);
        salvoTurretTexture = LoadTextureFromImage(salvoTurretIMG);
        UnloadImage(salvoTurretIMG);
    }
    static void DestroyTextures()
    {
        // unload base
        UnloadTexture(turretBaseTexture);
        // unload basic_turret
        UnloadTexture(duoTurretTexture);
        // unload laser_turret
        UnloadTexture(lancerTurretTexture);
    }

    /* A lot of declarations
     * static variables, independent of instance.
     * but the problem is
     * when we call static int x; the compiler knows it's a static member, but doesn't allocate memory for it (happens in classes)
     * You would have to initialise this member OUTSIDE ANY OTHER CLASS, in a global scope, by saying int x = <something>; again.
     * Solved : inline keyword.
     * Allocates memory at declaration. Compiler now knows it's a static variable that's declared here; it doesn't look for a definition OUTSIDE any class, and it can be defined at any point inside the class like another normal variable.
     * can be accessed from outside of class irrespective of instance of class also
     */
    // the base of T-1, T-2, turrets. Common
    inline static Image turretBaseIMG;
    inline static Texture2D turretBaseTexture;
    // the base of T-3 Turrets;
    inline static Image turretBaseIMG_2;
    inline static Texture2D turretBaseTexture_2;
    // ---- PROJECTILE TURRET STUFF ----
    // Duo turret
    inline static Texture2D duoTurretTexture;
    // Ripple turret
    inline static Image rippleTurretIMG;
    inline static Texture2D rippleTurretTexture;
    // smite turret
    inline static Image smiteTurretIMG;
    inline static Texture2D smiteTurretTexture;

    // ---- LASER TURRET STUFF ----
    // lancer
    inline static Image lancerTurretIMG;
    inline static Texture2D lancerTurretTexture;
    // cyclone
    inline static Image cycloneTurretIMG;
    inline static Texture2D cycloneTurretTexture;
    // meltdown
    inline static Image meltdownTurretIMG;
    inline static Texture2D meltdownTurretTexture;

    // ---- STATUS EFFECT TURRET ----
    inline static Image waveTurretIMG;
    inline static Texture2D waveTurretTexture;

    inline static Image salvoTurretIMG;
    inline static Texture2D salvoTurretTexture;

  protected:
    inline static float y_offset = 200; // help in drawing
  private:
    float projectileSpeed;

    /* Aim functionality
     * solves a simple quadratic equation
     * to predict the intersection point of projectile and enemy
     * using the values of their speed and position
     */
    Vector2 CalculateInterceptPoint(Vector2 enemyPos, Vector2 enemyVel, float projectileSpeed)
    {
        Vector2 toEnemy = enemyPos - position;
        float a = Vector2LengthSqr(enemyVel) - projectileSpeed * projectileSpeed;
        float b = 2 * Vector2DotProduct(toEnemy, enemyVel);
        float c = Vector2LengthSqr(toEnemy);
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0 || fabs(a) < 0.0001f)
        {
            return enemyPos; // No solution, aim directly at the enemy
        }
        else
        {
            float sqrtDisc = sqrt(discriminant);
            float t1 = (-b - sqrtDisc) / (2 * a);
            float t2 = (-b + sqrtDisc) / (2 * a);
            float t = (t1 > 0) ? t1 : ((t2 > 0) ? t2 : -1);

            if (t > 0)
            {
                return Vector2Add(enemyPos, Vector2Scale(enemyVel, t));
            }
            else
            {
                return enemyPos;
            }
        }
    }
    /* This fails for the lancer turret
     * This function does not take into consideration
     * the time required for the projectile to spawn at the location of turret
     * hence it is inaccurate
     * A necessary debuff, in my opinion
     * Starter turrets should be weak;
     */

    virtual float GetRotationSpeed() = 0;
};
// --------------------------------------------

/* BASIC TURRET SYSTEMS
 * In reverse order of power, strongest first
 * this pattern was chosen to accomodate for upgrade paths, that require the stronger turret to be defined first
 * so that the weaker can run a make_unique<StrongerTurret> in its upgrade function
 */

/*  Strongest projectile turret
 * Fires multiple projectiles in a spread pattern
 * High damage output, but slow fire rate
 * Intended to be good at crowd control
 */
class smite_turret : public Turret
{
  public:
    smite_turret(Vector2 pos, Tile *tile) : Turret(pos, tile, shotgun_bullet_speed, TurretType::SMITE)
    {
        range = smite_turret_range;
        cooldownTimer = 1 / smite_turret_fire_rate;
        fireTimer = 0.0f;
        rotationSpeed = 15.0f;
        m_recoilOffset = 20.0f;
    }
    void Draw() override
    {
        Vector2 baseOrigin = {(float)turretBaseTexture_2.width / 2.0f, (float)turretBaseTexture_2.height / 2.0f};

        DrawTexturePro(turretBaseTexture_2, {0, 0, (float)turretBaseTexture_2.width, (float)turretBaseTexture_2.height}, {position.x, position.y, (float)turretBaseTexture_2.width, (float)turretBaseTexture_2.height}, baseOrigin, 0.0f, WHITE);

        float angleRad = gunRotation * DEG2RAD;
        Vector2 direction = {cosf(angleRad), sinf(angleRad)};
        Vector2 gunDrawPosition = Vector2Subtract(position, Vector2Scale(direction, recoilOffset));
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)smiteTurretTexture.width, (float)smiteTurretTexture.height};

        Vector2 gunOrigin = {(float)smiteTurretTexture.width / 2.0f, (float)smiteTurretTexture.height / 2.0f};
        DrawTexturePro(smiteTurretTexture, {0, 0, (float)smiteTurretTexture.width, (float)smiteTurretTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
    }
    bool DrawTurretInfo(vector<unique_ptr<Entity>> &entities) override
    {
        DrawTexturePro(smiteTurretTexture, {0, 0, (float)smiteTurretTexture.width, (float)smiteTurretTexture.height}, {GRID_COLS * TILE_SIZE + 160, y_offset - 70, 2 * (float)duoTurretTexture.width, 2 * (float)duoTurretTexture.height}, {(float)duoTurretTexture.width / 2, (float)duoTurretTexture.height / 2}, 0.0f, WHITE);
        DrawText("--- SMITE ---", GRID_COLS * TILE_SIZE + 60, y_offset - 20, 40, BLACK);
        DrawText("Level : 3", GRID_COLS * TILE_SIZE + 140, y_offset + 20, 20, BLACK);
        DrawText("Projectiles : 10", GRID_COLS * TILE_SIZE + 140, y_offset + 40, 20, BLACK);
        DrawText(TextFormat("Dmg/Proj : %.1f", shotgun_bullet_damage), GRID_COLS * TILE_SIZE + 140, y_offset + 60, 20, BLACK);
        DrawText("Pierce count : 3", GRID_COLS * TILE_SIZE + 140, y_offset + 80, 20, BLACK);
        DrawText(TextFormat("Fire rate : %d", (int)smite_turret_fire_rate), GRID_COLS * TILE_SIZE + 140, y_offset + 100, 20, BLACK);
        DrawText(TextFormat("Range : %d tiles", (int)(smite_turret_range / TILE_SIZE)), GRID_COLS * TILE_SIZE + 140, y_offset + 120, 20, BLACK);
        DrawText("Spread : 30 degrees", GRID_COLS * TILE_SIZE + 140, y_offset + 140, 20, BLACK);
        Rectangle sellButton = {GRID_COLS * TILE_SIZE + 150, y_offset + 180, 120, 40};
        if (GuiButton(sellButton, TextFormat("Sell (%d)", smite_turret_cost - 200)))
        {
            this->tileOfTurret->hasTurret = false;
            playerMoney += (smite_turret_cost - 200);
            this->Destroy();
            return true;
        }
        return false;
    }
    float GetRotationSpeed() override { return rotationSpeed; }
};

/* A flamethrower type turret => Tier 2 of projectile turret
 * While the core logic remains as the same as of duo_turret
 * It fires special projectiles at a very high rate
 * giving it the flamethrower like look
 * Intended to be moderate at crowd control and damage output
 */
class ripple_turret : public Turret
{
  public:
    ripple_turret(Vector2 pos, Tile *tile) : Turret(pos, tile, flame_bullet_speed, TurretType::RIPPLE)
    {
        range = ripple_turret_range;
        cooldownTimer = 1 / ripple_turret_fire_rate;
        fireTimer = 0.0f; // initial timer, ready to fire
        rotationSpeed = 15.0f;
        m_recoilOffset = 10.0f;
    }

    void Draw() override
    {
        Vector2 baseOrigin = {(float)turretBaseTexture.width / 2.0f, (float)turretBaseTexture.height / 2.0f};

        DrawTexturePro(turretBaseTexture, {0, 0, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, {position.x, position.y, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, baseOrigin, 0.0f, WHITE);

        float angleRad = gunRotation * DEG2RAD;
        Vector2 direction = {cosf(angleRad), sinf(angleRad)};
        Vector2 gunDrawPosition = Vector2Subtract(position, Vector2Scale(direction, recoilOffset));
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)rippleTurretTexture.width, (float)rippleTurretTexture.height};

        Vector2 gunOrigin = {(float)rippleTurretTexture.width / 2.0f, (float)rippleTurretTexture.height / 2.0f};
        DrawTexturePro(rippleTurretTexture, {0, 0, (float)rippleTurretTexture.width, (float)rippleTurretTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
    }
    bool DrawTurretInfo(vector<unique_ptr<Entity>> &entities) override
    {
        DrawTexturePro(rippleTurretTexture, {0, 0, (float)rippleTurretTexture.width, (float)rippleTurretTexture.height}, {GRID_COLS * TILE_SIZE + 160, y_offset - 70, 2 * (float)duoTurretTexture.width, 2 * (float)duoTurretTexture.height}, {(float)duoTurretTexture.width / 2, (float)duoTurretTexture.height / 2}, 0.0f, WHITE);
        DrawText("--- RIPPLE ---", GRID_COLS * TILE_SIZE + 40, y_offset - 20, 40, BLACK);
        DrawText("Level : 2", GRID_COLS * TILE_SIZE + 140, y_offset + 20, 20, BLACK);
        DrawText("Projectiles : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 40, 20, BLACK);
        DrawText(TextFormat("Dmg/Proj : %.1f", flame_bullet_damage), GRID_COLS * TILE_SIZE + 140, y_offset + 60, 20, BLACK);
        DrawText(TextFormat("Fire rate : %d", (int)ripple_turret_fire_rate), GRID_COLS * TILE_SIZE + 140, y_offset + 80, 20, BLACK);
        DrawText(TextFormat("Range : %d tiles", (int)(ripple_turret_range / TILE_SIZE)), GRID_COLS * TILE_SIZE + 140, y_offset + 100, 20, BLACK);
        DrawText(TextFormat("Spread : %.1f deg", flame_bullet_spread), GRID_COLS * TILE_SIZE + 140, y_offset + 120, 20, BLACK);
        Rectangle upgradeButton = {GRID_COLS * TILE_SIZE + 80, y_offset + 160, 120, 40};
        if (GuiButton(upgradeButton, TextFormat("Upgrade (%d)", smite_turret_cost)) && playerMoney >= smite_turret_cost)
        {
            entities.push_back(make_unique<smite_turret>(this->position, this->tileOfTurret));
            playerMoney -= smite_turret_cost;
            stat_manager.MoneySpent(smite_turret_cost);
            this->Destroy();
            return true;
        }
        Rectangle sellButton = {GRID_COLS * TILE_SIZE + 220, y_offset + 160, 120, 40};
        if (GuiButton(sellButton, TextFormat("Sell (%d)", ripple_turret_cost - 100)))
        {
            this->tileOfTurret->hasTurret = false;
            playerMoney += (ripple_turret_cost - 100);
            this->Destroy();
            return true;
        }

        return false;
    }

    

  private:
    float GetRotationSpeed() override { return rotationSpeed; }
};

/* The basic projectile firing turret
 * Fires projectile called normal_bullet in a straight line
 * at predicted position of enemy
 * Intended to be a starter turret, weak at everything
 */
class duo_turret : public Turret
{

  public:
    duo_turret(Vector2 pos, Tile *tile) : Turret(pos, tile, normal_bullet_speed, TurretType::DUO)
    {
        range = duo_turret_range;
        cooldownTimer = 1 / duo_turret_fire_rate;
        fireTimer = 0.0f; // initial timer, ready to fire
        rotationSpeed = 5.0f;
        m_recoilOffset = 4.0f;
    }

    void Draw() override
    {
        Vector2 baseOrigin = {(float)turretBaseTexture.width / 2.0f, (float)turretBaseTexture.height / 2.0f};

        DrawTexturePro(turretBaseTexture, {0, 0, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, {position.x, position.y, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, baseOrigin, 0.0f, WHITE);

        // calculating change in gun_rec to account for recoil
        float angleRad = gunRotation * DEG2RAD;
        Vector2 direction = {cosf(angleRad), sinf(angleRad)};
        Vector2 gunDrawPosition = Vector2Subtract(position, Vector2Scale(direction, recoilOffset));
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)duoTurretTexture.width, (float)duoTurretTexture.height};

        Vector2 gunOrigin = {(float)duoTurretTexture.width / 2.0f, (float)duoTurretTexture.height * 0.75f - 2.0f};
        DrawTexturePro(duoTurretTexture, {0, 0, (float)duoTurretTexture.width, (float)duoTurretTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
    }

    bool DrawTurretInfo(vector<unique_ptr<Entity>> &entities) override
    {
        DrawTexturePro(duoTurretTexture, {0, 0, (float)duoTurretTexture.width, (float)duoTurretTexture.height}, {GRID_COLS * TILE_SIZE + 160, y_offset - 70, 2 * (float)duoTurretTexture.width, 2 * (float)duoTurretTexture.height}, {(float)duoTurretTexture.width / 2, (float)duoTurretTexture.height / 2}, 0.0f, WHITE);
        DrawText("--- DUO ---", GRID_COLS * TILE_SIZE + 60, y_offset - 20, 40, BLACK);
        DrawText("Level : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 20, 20, BLACK);
        DrawText("Projectiles : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 40, 20, BLACK);
        DrawText(TextFormat("Dmg/Proj : %d", (int)normal_bullet_damage), GRID_COLS * TILE_SIZE + 140, y_offset + 60, 20, BLACK);
        DrawText(TextFormat("Fire rate : %d", (int)duo_turret_fire_rate), GRID_COLS * TILE_SIZE + 140, y_offset + 80, 20, BLACK);
        DrawText(TextFormat("Range : %d tiles", (int)(duo_turret_range / TILE_SIZE)), GRID_COLS * TILE_SIZE + 140, y_offset + 100, 20, BLACK);
        DrawText("Spread : 0", GRID_COLS * TILE_SIZE + 140, y_offset + 120, 20, BLACK);
        Rectangle upgradeButton = {GRID_COLS * TILE_SIZE + 80, y_offset + 160, 120, 40};
        if (GuiButton(upgradeButton, TextFormat("Upgrade (%d)", ripple_turret_cost)) && playerMoney >= ripple_turret_cost)
        {
            entities.push_back(make_unique<ripple_turret>(this->position, this->tileOfTurret));
            playerMoney -= ripple_turret_cost;
            stat_manager.MoneySpent(ripple_turret_cost);
            this->Destroy();
            return true;
        }
        Rectangle sellButton = {GRID_COLS * TILE_SIZE + 220, y_offset + 160, 120, 40};
        if (GuiButton(sellButton, TextFormat("Sell (%d)", duo_turret_cost - 20)))
        {
            this->tileOfTurret->hasTurret = false;
            playerMoney += (duo_turret_cost - 20);
            this->Destroy();
            return true;
        }
        return false;
    }


    static void DrawBuildInfo()
    {
        DrawTexturePro(duoTurretTexture, {0, 0, (float)duoTurretTexture.width, (float)duoTurretTexture.height}, {GRID_COLS * TILE_SIZE + 160, y_offset - 70, 2 * (float)duoTurretTexture.width, 2 * (float)duoTurretTexture.height}, {(float)duoTurretTexture.width / 2, (float)duoTurretTexture.height / 2}, 0.0f, WHITE);
        DrawText("--- DUO ---", GRID_COLS * TILE_SIZE + 60, y_offset - 20, 40, BLACK);
        DrawText("Level : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 20, 20, BLACK);
        DrawText("Projectiles : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 40, 20, BLACK);
        DrawText(TextFormat("Dmg/Proj : %d", (int)normal_bullet_damage), GRID_COLS * TILE_SIZE + 140, y_offset + 60, 20, BLACK);
        DrawText(TextFormat("Fire rate : %d", (int)duo_turret_fire_rate), GRID_COLS * TILE_SIZE + 140, y_offset + 80, 20, BLACK);
        DrawText(TextFormat("Range : %d tiles", (int)(duo_turret_range / TILE_SIZE)), GRID_COLS * TILE_SIZE + 140, y_offset + 100, 20, BLACK);
        DrawText("Spread : 0", GRID_COLS * TILE_SIZE + 140, y_offset + 120, 20, BLACK);
        DrawText(TextFormat("Cost: %d", duo_turret_cost), GRID_COLS * TILE_SIZE + 140, y_offset + 160, 20, DARKGREEN);
    }

  private:
    float GetRotationSpeed() override { return rotationSpeed; }
};

// -----------------------------------

/* LASER TURRET SYSTEMS
 * INVOLVES THREE TURRETS, IN REVERSE ORDER OF UPGRADE
 */

/* Third Turret :
 * Fires a continuous sustained beam from source to dest for a few seconds, after which cools down.
 * Damages everything in the path.
 * High damage output at disadvantage of cooldown
 * very good at single target damage
 * but beam is thick enough to hit multiple targets
 * end-game turret
 */
class meltdown_turret : public Turret
{
  public:
    float cooldown_timer;
    float beam_timer;
    bool is_active;
    Vector2 target_pos;
    float recoil_velocity = 100.0f;
    float beamThickness = 10.0f;
    meltdown_turret(Vector2 pos, Tile *tile) : Turret(pos, tile, 0.0f, TurretType::MELTDOWN)
    {
        range = meltdown_turret_range;
        rotationSpeed = 8.0f;
        m_recoilOffset = 10.0f;
    }
    void Update(float deltaTime) override
    {
        if (is_active)
        {
            beam_timer -= deltaTime;
            if (recoilOffset <= m_recoilOffset)
            {
                recoilOffset += recoil_velocity * deltaTime;
            }
            if (beam_timer <= 0)
            {
                is_active = false;
                cooldown_timer = meltdown_turret_cooldown_timer;
                beam_timer = 0;
            }
        }
        else
        {
            cooldown_timer -= deltaTime;
            recoilOffset = Lerp(recoilOffset, 0.0f, 0.1f);
        }
    }
    void Update(float deltaTime, const std::vector<Enemy *> &targets, std::vector<std::unique_ptr<Entity>> &newProjectiles) override
    {

        float min_dist = 99999.0f;
        Enemy *target_ptr = nullptr;
        for (auto &enemy : targets)
        {
            if (!enemy->isVisible)
                continue;
            if (Vector2DistanceSqr(enemy->position, this->position) <= range * range)
            {
                if (Vector2DistanceSqr(position, enemy->GetPosition()) <= min_dist)
                {
                    target_ptr = enemy;
                    min_dist = Vector2DistanceSqr(position, enemy->GetPosition());
                }
            }
        }
        if (target_ptr)
        {
            Vector2 aimPoint = target_ptr->GetPosition();
            float targetAngle = atan2f(aimPoint.y - position.y, aimPoint.x - position.x) * RAD2DEG;
            gunRotation = MoveAngle(gunRotation, targetAngle, rotationSpeed);
            float angleDifference = normaliseAngle(targetAngle - gunRotation);
            if (angleDifference <= 10.0f && cooldown_timer <= 0)
            {
                target_pos = position + Vector2Scale(Vector2Normalize(aimPoint - position), range);
                is_active = true;
                if (beam_timer <= 0)
                {
                    beam_timer = meltdown_turret_beam_timer;
                }
                cooldown_timer = 0;
                for (auto &enemy : targets)
                {
                    if (CheckCollisionCircleLine(enemy->GetPosition(), enemy->GetRadius() + beamThickness / 2.0f, position, target_pos))
                    {
                        enemy->TakeDamageByValue(ProjectileType::MELTDOWN_BEAM, meltdown_turret_dps * deltaTime);
                    }
                }
            }
        }
    }

    void Draw() override
    {
        DrawTexturePro(turretBaseTexture_2, {0, 0, (float)turretBaseTexture_2.width, (float)turretBaseTexture_2.height}, {position.x, position.y, (float)turretBaseTexture_2.width, (float)turretBaseTexture_2.height}, {(float)turretBaseTexture_2.width / 2.0f, (float)turretBaseTexture_2.height / 2.0f}, 0.0f, WHITE);

        if (is_active)
        {
            BeginBlendMode(BLEND_ADDITIVE);
            float lifeRatio = beam_timer / cyclone_turret_beam_timer;
            float alpha = lifeRatio * 0.8f;
            DrawLineEx(position, target_pos, beamThickness, Fade(ORANGE, alpha * 0.2f));
            DrawLineEx(position, target_pos, beamThickness * 0.8f, Fade(YELLOW, alpha * 0.5f));
            DrawLineEx(position, target_pos, beamThickness * 0.2f, Fade(WHITE, alpha));
            EndBlendMode();
            // different timers for fading out of different sections of beams give it a smooth outward -> inward fade out effect.
        }
        float angleRad = gunRotation * DEG2RAD;
        Vector2 direction = {cosf(angleRad), sinf(angleRad)};
        Vector2 gunDrawPosition = Vector2Subtract(position, Vector2Scale(direction, recoilOffset));
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)meltdownTurretTexture.width, (float)meltdownTurretTexture.height};

        Vector2 gunOrigin = {(float)meltdownTurretTexture.width / 2.0f, (float)meltdownTurretTexture.height * 0.75f - 2.0f};
        DrawTexturePro(meltdownTurretTexture, {0, 0, (float)meltdownTurretTexture.width, (float)meltdownTurretTexture.height}, gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
        // I don't know why i declared so many intermediate variables. But it's done now and i'm too lazy to revert. Good bye, optimised RAM usage. (please have lots of ram)
    }
    bool DrawTurretInfo(vector<unique_ptr<Entity>> &entities) override
    {
        DrawTexturePro(meltdownTurretTexture, {0, 0, (float)meltdownTurretTexture.width, (float)meltdownTurretTexture.height}, {GRID_COLS * TILE_SIZE + 160, y_offset - 70, 2 * (float)duoTurretTexture.width, 2 * (float)duoTurretTexture.height}, {(float)duoTurretTexture.width / 2, (float)duoTurretTexture.height / 2}, 0.0f, WHITE);
        DrawText("--- MELTDOWN ---", GRID_COLS * TILE_SIZE + 20, y_offset - 20, 40, BLACK);
        DrawText("Level : 3", GRID_COLS * TILE_SIZE + 140, y_offset + 20, 20, BLACK);
        DrawText(TextFormat("Base damage/s : %.1f", meltdown_turret_dps), GRID_COLS * TILE_SIZE + 140, y_offset + 60, 20, BLACK);
        DrawText(TextFormat("Beam timer : %.1f", meltdown_turret_beam_timer), GRID_COLS * TILE_SIZE + 140, y_offset + 80, 20, BLACK);
        DrawText(TextFormat("Cooldown Timer : %.1f", meltdown_turret_cooldown_timer), GRID_COLS * TILE_SIZE + 140, y_offset + 100, 20, BLACK);
        DrawText(TextFormat("Range : %d tiles", (int)(meltdown_turret_range / TILE_SIZE)), GRID_COLS * TILE_SIZE + 140, y_offset + 120, 20, BLACK);
        Rectangle sellButton = {GRID_COLS * TILE_SIZE + 150, y_offset + 160, 120, 40};
        if (GuiButton(sellButton, TextFormat("Sell (%d)", meltdown_turret_cost - 300)))
        {
            this->tileOfTurret->hasTurret = false;
            playerMoney += (meltdown_turret_cost - 300);
            this->Destroy();
            return true;
        }
        return false;
    }

    

  private:
    float GetRotationSpeed() override { return rotationSpeed; }
};

/* Second Turret : Fires an instantaneous bolt from source
 * in the direction of closest enemy with bolt extending
 * all the way till the maximum range, damaging everything along the way
 */
class cyclone_turret : public Turret
{
  public:
    float cooldown_timer;
    float beam_timer;
    bool is_active;

    Vector2 target_pos;
    cyclone_turret(Vector2 pos, Tile *tile) : Turret(pos, tile, 0.0f, TurretType::CYCLONE)
    {
        range = cyclone_turret_range;
        rotationSpeed = 5.0f;
        m_recoilOffset = 5.0f;
    }
    void Update(float deltaTime) override
    {
        if (is_active)
        {
            beam_timer -= deltaTime;
            recoilOffset = m_recoilOffset;
            if (beam_timer <= 0)
            {
                is_active = false;
                cooldown_timer = cyclone_turret_cooldown_timer;
                beam_timer = 0;
            }
        }
        else
        {
            cooldown_timer -= deltaTime;
            recoilOffset = Lerp(recoilOffset, 0.0f, 0.5f);
        }
    }
    void Update(float deltaTime, const std::vector<Enemy *> &targets, std::vector<std::unique_ptr<Entity>> &newProjectiles) override
    {
        if (!is_active)
        {
            float min_dist = 99999.0f;
            Enemy *target_ptr = nullptr;
            for (auto &enemy : targets)
            {
                if (!enemy->isVisible)
                    continue;
                if (Vector2DistanceSqr(enemy->position, this->position) <= range * range)
                {
                    if (Vector2DistanceSqr(position, enemy->GetPosition()) <= min_dist)
                    {
                        target_ptr = enemy;
                        min_dist = Vector2DistanceSqr(position, enemy->GetPosition());
                    }
                }
            }
            if (target_ptr)
            {
                Vector2 aimPoint = target_ptr->GetPosition();
                float targetAngle = atan2f(aimPoint.y - position.y, aimPoint.x - position.x) * RAD2DEG;
                gunRotation = MoveAngle(gunRotation, targetAngle, rotationSpeed);
                float angleDifference = normaliseAngle(targetAngle - gunRotation);
                if (angleDifference <= 5.0f && cooldown_timer <= 0)
                {
                    target_pos = position + Vector2Scale(Vector2Normalize(aimPoint - position), range);
                    is_active = true;
                    cooldown_timer = 0;
                    beam_timer = cyclone_turret_beam_timer;
                    int enemy_hit = 0;
                    for (auto &enemy : targets)
                    {
                        if (enemy_hit > cyclone_turret_max_pierce_count)
                        {
                            break;
                        }
                        if (CheckCollisionCircleLine(enemy->GetPosition(), enemy->GetRadius(), position, target_pos))
                        {
                            enemy->TakeDamage(ProjectileType::CYCLONE_BEAM, GetDamageFalloff(Vector2DistanceSqr(enemy->GetPosition(), position), range, enemy_hit));
                            enemy_hit++;
                        }
                    }
                }
            }
        }
    }

    void Draw() override
    {
        DrawTexturePro(turretBaseTexture, {0, 0, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, {position.x, position.y, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, {(float)turretBaseTexture.width / 2.0f, (float)turretBaseTexture.height / 2.0f}, 0.0f, WHITE);

        float angleRad = gunRotation * DEG2RAD;
        Vector2 direction = {cosf(angleRad), sinf(angleRad)};
        Vector2 gunDrawPosition = Vector2Subtract(position, Vector2Scale(direction, recoilOffset));
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)cycloneTurretTexture.width, (float)cycloneTurretTexture.height};

        Vector2 gunOrigin = {(float)cycloneTurretTexture.width / 2.0f, (float)cycloneTurretTexture.height * 0.75f - 2.0f};
        DrawTexturePro(cycloneTurretTexture, {0, 0, (float)cycloneTurretTexture.width, (float)cycloneTurretTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);

        if (is_active)
        {
            BeginBlendMode(BLEND_ADD_COLORS);

            float lifeRatio = beam_timer / cyclone_turret_beam_timer;

            DrawLineEx(position, target_pos, 1.0f, Fade(WHITE, lifeRatio));

            DrawLineEx(position, target_pos, 3.0f, Fade(YELLOW, lifeRatio * 0.6f));

            EndBlendMode();
        }
    }
    bool DrawTurretInfo(vector<unique_ptr<Entity>> &entities) override
    {
        DrawTexturePro(cycloneTurretTexture, {0, 0, (float)cycloneTurretTexture.width, (float)cycloneTurretTexture.height}, {GRID_COLS * TILE_SIZE + 160, y_offset - 70, 2 * (float)duoTurretTexture.width, 2 * (float)duoTurretTexture.height}, {(float)duoTurretTexture.width / 2, (float)duoTurretTexture.height / 2}, 0.0f, WHITE);
        DrawText("--- CYCLONE ---", GRID_COLS * TILE_SIZE + 60, y_offset - 20, 40, BLACK);
        DrawText("Level : 2", GRID_COLS * TILE_SIZE + 140, y_offset + 20, 20, BLACK);
        DrawText("Max pierce count : 3", GRID_COLS * TILE_SIZE + 140, y_offset + 40, 20, BLACK);
        DrawText(TextFormat("Base damage : %.1f", cyclone_beam_damage), GRID_COLS * TILE_SIZE + 140, y_offset + 60, 20, BLACK);
        DrawText(TextFormat("Fire rate : %.2f", 1 / (cyclone_turret_cooldown_timer + cyclone_turret_beam_timer)), GRID_COLS * TILE_SIZE + 140, y_offset + 100, 20, BLACK);
        DrawText(TextFormat("Range : %d tiles", (int)(cyclone_turret_range / TILE_SIZE)), GRID_COLS * TILE_SIZE + 140, y_offset + 120, 20, BLACK);
        Rectangle upgradeButton = {GRID_COLS * TILE_SIZE + 80, y_offset + 160, 120, 40};
        if (GuiButton(upgradeButton, TextFormat("Upgrade (%d)", meltdown_turret_cost)) && playerMoney >= meltdown_turret_cost)
        {
            entities.push_back(make_unique<meltdown_turret>(this->position, this->tileOfTurret));
            playerMoney -= meltdown_turret_cost;
            stat_manager.MoneySpent(meltdown_turret_cost);
            this->Destroy();
            return true;
        }
        Rectangle sellButton = {GRID_COLS * TILE_SIZE + 220, y_offset + 160, 120, 40};
        if (GuiButton(sellButton, TextFormat("Sell (%d)", cyclone_turret_cost - 200)))
        {
            this->tileOfTurret->hasTurret = false;
            playerMoney += (cyclone_turret_cost - 200);
            this->Destroy();
            return true;
        }
        return false;
    }

  private:
    float GetRotationSpeed() override { return rotationSpeed; }
};

/* First turret : Fires a laser bolt, a fast moving projectile
 * that pierces through multiple enemies
 * Intended to be a high damage single target turret
 * high innacuracy in the firing mechanism (unintended but accepted)
 */
class lancer_turret : public Turret
{
  public:
    lancer_turret(Vector2 pos, Tile *tile) : Turret(pos, tile, lancer_bullet_speed, TurretType::LANCER)
    {
        range = lancer_turret_range;
        cooldownTimer = 1 / lancer_turret_fire_rate;
        fireTimer = 0.0f; // initial timer, ready to fire
        rotationSpeed = 2.5f;
        m_recoilOffset = 8.0f;
    }
    void Draw() override
    {

        DrawTexturePro(turretBaseTexture, {0, 0, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, {position.x, position.y, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, {(float)turretBaseTexture.width / 2.0f, (float)turretBaseTexture.height / 2.0f}, 0.0f, WHITE);

        // calculating change in gun_rec to account for recoil

        float angleRad = gunRotation * DEG2RAD;
        Vector2 direction = {cosf(angleRad), sinf(angleRad)};
        Vector2 gunDrawPosition = Vector2Subtract(position, Vector2Scale(direction, recoilOffset));
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)lancerTurretTexture.width, (float)lancerTurretTexture.height};

        Vector2 gunOrigin = {(float)lancerTurretTexture.width / 2.0f, (float)lancerTurretTexture.height * 0.75f - 2.0f};
        DrawTexturePro(lancerTurretTexture, {0, 0, (float)lancerTurretTexture.width, (float)lancerTurretTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
    }
    bool DrawTurretInfo(vector<unique_ptr<Entity>> &entities) override
    {
        DrawTexturePro(lancerTurretTexture, {0, 0, (float)lancerTurretTexture.width, (float)lancerTurretTexture.height}, {GRID_COLS * TILE_SIZE + 160, y_offset - 70, 2 * (float)duoTurretTexture.width, 2 * (float)duoTurretTexture.height}, {(float)duoTurretTexture.width / 2, (float)duoTurretTexture.height / 2}, 0.0f, WHITE);
        DrawText("--- LANCER ---", GRID_COLS * TILE_SIZE + 60, y_offset - 20, 40, BLACK);
        DrawText("Level : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 20, 20, BLACK);
        DrawText("Projectiles : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 40, 20, BLACK);
        DrawText(TextFormat("Dmg/Proj : %.1f", lancer_bullet_damage), GRID_COLS * TILE_SIZE + 140, y_offset + 60, 20, BLACK);
        DrawText("Pierce count : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 80, 20, BLACK);
        DrawText(TextFormat("Fire rate : %d", (int)lancer_turret_fire_rate), GRID_COLS * TILE_SIZE + 140, y_offset + 100, 20, BLACK);
        DrawText(TextFormat("Range : %d tiles", (int)(lancer_turret_range / TILE_SIZE)), GRID_COLS * TILE_SIZE + 140, y_offset + 120, 20, BLACK);
        Rectangle upgradeButton = {GRID_COLS * TILE_SIZE + 80, y_offset + 160, 120, 40};
        if (GuiButton(upgradeButton, TextFormat("Upgrade (%d)", cyclone_turret_cost)) && playerMoney >= cyclone_turret_cost)
        {
            entities.push_back(make_unique<cyclone_turret>(this->position, this->tileOfTurret));
            playerMoney -= cyclone_turret_cost;
            stat_manager.MoneySpent(cyclone_turret_cost);
            this->Destroy();
            return true;
        }
        Rectangle sellButton = {GRID_COLS * TILE_SIZE + 220, y_offset + 160, 120, 40};
        if (GuiButton(sellButton, TextFormat("Sell (%d)", lancer_turret_cost - 50)))
        {
            this->tileOfTurret->hasTurret = false;
            playerMoney += (lancer_turret_cost - 50);
            this->Destroy();
            return true;
        }

        return false;
    }
    static void DrawBuildInfo()
    {
        DrawTexturePro(lancerTurretTexture, {0, 0, (float)lancerTurretTexture.width, (float)lancerTurretTexture.height}, {GRID_COLS * TILE_SIZE + 160, y_offset - 70, 2 * (float)duoTurretTexture.width, 2 * (float)duoTurretTexture.height}, {(float)duoTurretTexture.width / 2, (float)duoTurretTexture.height / 2}, 0.0f, WHITE);
        DrawText("--- LANCER ---", GRID_COLS * TILE_SIZE + 60, y_offset - 20, 40, BLACK);
        DrawText("Level : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 20, 20, BLACK);
        DrawText("Projectiles : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 40, 20, BLACK);
        DrawText(TextFormat("Dmg/Proj : %.1f", lancer_bullet_damage), GRID_COLS * TILE_SIZE + 140, y_offset + 60, 20, BLACK);
        DrawText("Pierce count : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 80, 20, BLACK);
        DrawText(TextFormat("Fire rate : %d", (int)lancer_turret_fire_rate), GRID_COLS * TILE_SIZE + 140, y_offset + 100, 20, BLACK);
        DrawText(TextFormat("Range : %d tiles", (int)(lancer_turret_range / TILE_SIZE)), GRID_COLS * TILE_SIZE + 140, y_offset + 120, 20, BLACK);
        DrawText(TextFormat("Cost: %d", lancer_turret_cost), GRID_COLS * TILE_SIZE + 140, y_offset + 160, 20, DARKGREEN);
    }

  private:
    float GetRotationSpeed() override { return rotationSpeed; }
};

// ----------------------------------------

/* SLOWING TURRET SYSTEMS
 * Plan to be updated, only prototype here
 */

class salvo_turret : public Turret
{
  public:
    salvo_turret(Vector2 pos, Tile *tile) : Turret(pos, tile, ice_stream_speed, TurretType::SALVO)
    {
        range = salvo_turret_range;
        cooldownTimer = 1 / salvo_turret_fire_rate;
        fireTimer = 0.0f; // initial timer, ready to fire
        rotationSpeed = 15.0f;
        m_recoilOffset = 5.0f;
    }

    void Draw() override
    {
        Vector2 baseOrigin = {(float)turretBaseTexture.width / 2.0f, (float)turretBaseTexture.height / 2.0f};

        DrawTexturePro(turretBaseTexture, {0, 0, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, {position.x, position.y, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, baseOrigin, 0.0f, WHITE);

        float angleRad = gunRotation * DEG2RAD;
        Vector2 direction = {cosf(angleRad), sinf(angleRad)};
        Vector2 gunDrawPosition = Vector2Subtract(position, Vector2Scale(direction, recoilOffset));
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)salvoTurretTexture.width, (float)salvoTurretTexture.height};

        Vector2 gunOrigin = {(float)salvoTurretTexture.width / 2.0f, (float)salvoTurretTexture.height * 0.75f - 2.0f};
        DrawTexturePro(salvoTurretTexture, {0, 0, (float)salvoTurretTexture.width, (float)salvoTurretTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
    }
    bool DrawTurretInfo(vector<unique_ptr<Entity>> &entities) override
    {
        DrawTexturePro(salvoTurretTexture, {0, 0, (float)salvoTurretTexture.width, (float)salvoTurretTexture.height}, {GRID_COLS * TILE_SIZE + 160, y_offset - 70, 2 * (float)duoTurretTexture.width, 2 * (float)duoTurretTexture.height}, {(float)duoTurretTexture.width / 2, (float)duoTurretTexture.height / 2}, 0.0f, WHITE);
        DrawText("--- SALVO ---", GRID_COLS * TILE_SIZE + 20, y_offset - 20, 40, BLACK);
        DrawText("Level : 2", GRID_COLS * TILE_SIZE + 140, y_offset + 20, 20, BLACK);
        DrawText(TextFormat("Damage/proj : %.1f", ice_stream_damage), GRID_COLS * TILE_SIZE + 140, y_offset + 60, 20, BLACK);
        DrawText(TextFormat("Fire rate : %.1f", salvo_turret_fire_rate), GRID_COLS * TILE_SIZE + 140, y_offset + 80, 20, BLACK);
        DrawText(TextFormat("Spread : %.1f deg", ice_stream_spread), GRID_COLS * TILE_SIZE + 140, y_offset + 100, 20, BLACK);
        DrawText(TextFormat("Range : %d tiles", (int)(meltdown_turret_range / TILE_SIZE)), GRID_COLS * TILE_SIZE + 140, y_offset + 120, 20, BLACK);
        Rectangle sellButton = {GRID_COLS * TILE_SIZE + 150, y_offset + 160, 120, 40};
        if (GuiButton(sellButton, TextFormat("Sell (%d)", salvo_turret_cost - 200)))
        {
            this->tileOfTurret->hasTurret = false;
            playerMoney += (salvo_turret_cost - 200);
            this->Destroy();
            return true;
        }

        return false;
    }

  private:
    float GetRotationSpeed() override { return rotationSpeed; }
};

/* First Turret :
 * Creates a zone that slows down enemies within its range
 * for a certain duration, after which it goes into cooldown
 * Prototype version, to be updated
 */
class wave_turret : public Turret
{
  public:
    float active_timer, cooldown_timer;
    bool is_active = false;
    wave_turret(Vector2 pos, Tile *tile) : Turret(pos, tile, 0.0f, TurretType::WAVE)
    {
        cooldown_timer = 0;
        this->active_timer = wave_turret_active_time;
        range = wave_turret_range;
    }
    void Update(float deltaTime) override
    {
        if (is_active)
        {
            active_timer -= deltaTime;
            if (active_timer <= 0)
            {
                is_active = false;
                cooldown_timer = wave_turret_cooldown_time;
            }
        }
        else
        {
            cooldown_timer -= deltaTime;
            if (cooldown_timer <= 0)
            {
                is_active = true;
                active_timer = wave_turret_active_time;
            }
        }
    }

    void Update(float deltaTime, const std::vector<Enemy *> &targets, std::vector<std::unique_ptr<Entity>> &newProjectiles) override
    {
        if (!is_active)
        {
            return;
        }

        for (auto &enemy : targets)
        {
            if (Vector2DistanceSqr(enemy->position, this->position) <= range * range)
            {
                enemy->status_effect = StatusEffects::SLOWED;
            }
        }
    }
    void Draw() override
    {
        Vector2 baseOrigin = {(float)turretBaseTexture.width / 2.0f, (float)turretBaseTexture.height / 2.0f};

        DrawTexturePro(turretBaseTexture, {0, 0, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, {position.x, position.y, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, baseOrigin, 0.0f, WHITE);

        DrawTexturePro(waveTurretTexture, {0, 0, (float)waveTurretTexture.width, (float)waveTurretTexture.height}, {position.x, position.y, (float)waveTurretTexture.width, (float)waveTurretTexture.height}, {(float)waveTurretTexture.width / 2, (float)waveTurretTexture.height / 2}, 0.0f, WHITE);

        if (is_active)
        {
            DrawCircleV(position, range, Fade(SKYBLUE, 0.2f));
        }
    }
    bool DrawTurretInfo(vector<unique_ptr<Entity>> &entities) override
    {
        DrawTexturePro(waveTurretTexture, {0, 0, (float)waveTurretTexture.width, (float)waveTurretTexture.height}, {GRID_COLS * TILE_SIZE + 160, y_offset - 70, 2 * (float)duoTurretTexture.width, 2 * (float)duoTurretTexture.height}, {(float)duoTurretTexture.width / 2, (float)duoTurretTexture.height / 2}, 0.0f, WHITE);
        DrawText("--- WAVE ---", GRID_COLS * TILE_SIZE + 20, y_offset - 20, 40, BLACK);
        DrawText("Level : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 20, 20, BLACK);
        DrawText("Base damage : 0", GRID_COLS * TILE_SIZE + 140, y_offset + 60, 20, BLACK);
        DrawText(TextFormat("Active timer : %.1f", wave_turret_active_time), GRID_COLS * TILE_SIZE + 140, y_offset + 80, 20, BLACK);
        DrawText(TextFormat("Cooldown Timer : %.1f", wave_turret_cooldown_time), GRID_COLS * TILE_SIZE + 140, y_offset + 100, 20, BLACK);
        DrawText(TextFormat("Range : %d tiles", (int)(wave_turret_range / TILE_SIZE)), GRID_COLS * TILE_SIZE + 140, y_offset + 120, 20, BLACK);
        Rectangle sellButton = {GRID_COLS * TILE_SIZE + 220, y_offset + 160, 120, 40};
        if (GuiButton(sellButton, TextFormat("Sell (%d)", wave_turret_cost - 100)))
        {
            this->tileOfTurret->hasTurret = false;
            playerMoney += (wave_turret_cost - 100);
            this->Destroy();
            return true;
        }
        Rectangle upgradeButton = {GRID_COLS * TILE_SIZE + 80, y_offset + 160, 120, 40};
        if (GuiButton(upgradeButton, TextFormat("Upgrade (%d)", salvo_turret_cost)) && playerMoney >= salvo_turret_cost)
        {
            entities.push_back(make_unique<salvo_turret>(this->position, this->tileOfTurret));
            playerMoney -= salvo_turret_cost;
            stat_manager.MoneySpent(salvo_turret_cost);
            this->Destroy();
            return true;
        }

        return false;
    }
    static void DrawBuildInfo()
    {
        DrawTexturePro(waveTurretTexture, {0, 0, (float)waveTurretTexture.width, (float)waveTurretTexture.height}, {GRID_COLS * TILE_SIZE + 160, y_offset - 70, 2 * (float)duoTurretTexture.width, 2 * (float)duoTurretTexture.height}, {(float)duoTurretTexture.width / 2, (float)duoTurretTexture.height / 2}, 0.0f, WHITE);
        DrawText("--- WAVE ---", GRID_COLS * TILE_SIZE + 60, y_offset - 20, 40, BLACK);
        DrawText("Level : 1", GRID_COLS * TILE_SIZE + 140, y_offset + 20, 20, BLACK);
        DrawText("Base damage : 0", GRID_COLS * TILE_SIZE + 140, y_offset + 60, 20, BLACK);
        DrawText(TextFormat("Active timer : %.1f", wave_turret_active_time), GRID_COLS * TILE_SIZE + 140, y_offset + 80, 20, BLACK);
        DrawText(TextFormat("Cooldown Timer : %.1f", wave_turret_cooldown_time), GRID_COLS * TILE_SIZE + 140, y_offset + 100, 20, BLACK);
        DrawText(TextFormat("Range : %d tiles", (int)(wave_turret_range / TILE_SIZE)), GRID_COLS * TILE_SIZE + 140, y_offset + 120, 20, BLACK);
        DrawText(TextFormat("Cost: %d", wave_turret_cost), GRID_COLS * TILE_SIZE + 140, y_offset + 160, 20, DARKGREEN);
    }

  private:
    float GetRotationSpeed() override { return rotationSpeed; }
};
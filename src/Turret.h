#pragma once
#include "Enemy.h"
#include "Entity.h"
#include "Map.h"
#include "Projectile.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include <Config.h>
#include <iostream>
#include <memory> // for std::unique_ptr
#include <vector>

enum class TurretType
{
    BASIC,
    LASER,
    SLOWING,
};

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
    Rectangle gunRec;
    float gunRotation; // degrees
    float rotationSpeed;
    float recoilOffset;
    float m_recoilOffset;
    Color gunColor;
    Tile tileOfTurret;
    TurretType turret;
    Turret(Vector2 pos, Tile &tile, float speed, TurretType t)
    {
        position = pos;     // position of the turret
        gunRotation = 0.0f; // initial rotation of the gun
        tileOfTurret = tile;
        projectileSpeed = speed;
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

            float rotationSpeed = 5.0f;
            gunRotation = MoveAngle(gunRotation, targetAngle, rotationSpeed);
            float angleDifference = normaliseAngle(targetAngle - gunRotation);
            if (fireTimer <= 0 && angleDifference < 8.0f)
            {

                switch (turret)
                {
                case TurretType::BASIC:
                {
                    newProjectiles.push_back(std::make_unique<normal_bullet>(position, aimPoint));
                    break;
                }
                case TurretType::LASER:
                {
                    newProjectiles.push_back(std::make_unique<laser_bullet>(position, aimPoint));
                    break;
                }
                }

                fireTimer = cooldownTimer;
                recoilOffset = m_recoilOffset;
            }
        }
    }
    virtual void Draw() override = 0;
    void drawRangeOnHover(Vector2 pos)
    {
        if (CheckCollisionPointRec(pos, tileOfTurret.rect))
        {
            DrawCircleLines(position.x, position.y, range, YELLOW);
        }
    }

    static void LoadTextures()
    {
        // load the base, common to all
        turretBaseIMG = LoadImage("assets/turrets/base-1.png");
        turretBaseTexture = LoadTextureFromImage(turretBaseIMG);
        UnloadImage(turretBaseIMG);

        // basic_turret
        duoTurretTexture = LoadTexture("assets/turrets/duo.png");

        // laser_turret
        scatterTurretIMG = LoadImage("assets/turrets/scatter.png");
        ImageResize(&scatterTurretIMG, TILE_SIZE, TILE_SIZE);
        scatterTurretTexture = LoadTextureFromImage(scatterTurretIMG);

        // slow turret
        tsunamiTurretIMG = LoadImage("assets/turrets/tsunami.png");
        ImageResize(&tsunamiTurretIMG, TILE_SIZE, TILE_SIZE);
        tsunamiTurretTX = LoadTextureFromImage(tsunamiTurretIMG);
    }
    static void DestroyTextures()
    {
        // unload base
        UnloadTexture(turretBaseTexture);
        // unload basic_turret
        UnloadTexture(duoTurretTexture);
        // unload laser_turret
        UnloadTexture(scatterTurretTexture);
    }

    //  protected:
    // the base of each turret. Common
    inline static Image turretBaseIMG;
    inline static Texture2D turretBaseTexture;

    // basic_turret (duo) stuff
    inline static Texture2D duoTurretTexture;

    // laser_turret stuff
    // scatter => lvl 1 laser turret
    inline static Image scatterTurretIMG;
    inline static Texture2D scatterTurretTexture;
    // cyclone
    inline static Image cycloneTurretIMG;
    inline static Texture2D cycloneTurretTexture;

    // slow turret (tsunami) stuff
    inline static Image tsunamiTurretIMG;
    inline static Texture2D tsunamiTurretTX;

  private:
    float projectileSpeed;

    // Aim functionality, can be made worse to be poor turret
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
};
// --------------------------------------------

/* BASIC TURRET SYSTEMS
 * THREE TURRETS, IN ORDER OF PROGRESSION
 */
class basic_turret_lvl1 : public Turret
{
  public:
    basic_turret_lvl1(Vector2 pos, Tile &tile) : Turret(pos, tile, normal_bullet_speed, TurretType::BASIC)
    {
        range = duo_turret_range;
        cooldownTimer = 1 / duo_turret_fire_rate;
        fireTimer = 0.0f; // initial timer, ready to fire
        rotationSpeed = 0.5f;
        m_recoilOffset = 4.0f;
    }

    void Draw() override
    {
        Vector2 baseOrigin = {(float)turretBaseTexture.width / 2.0f, (float)turretBaseTexture.height / 2.0f};

        DrawTexturePro(turretBaseTexture, {0, 0, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, {position.x, position.y, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, baseOrigin, 0.0f, WHITE);

        // calculating change in gun_rec to account for recoil
        //
        float angleRad = gunRotation * DEG2RAD;
        Vector2 direction = {cosf(angleRad), sinf(angleRad)};
        Vector2 gunDrawPosition = Vector2Subtract(position, Vector2Scale(direction, recoilOffset));
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)duoTurretTexture.width, (float)duoTurretTexture.height};

        Vector2 gunOrigin = {(float)duoTurretTexture.width / 2.0f, (float)duoTurretTexture.height * 0.75f - 2.0f};
        DrawTexturePro(duoTurretTexture, {0, 0, (float)duoTurretTexture.width, (float)duoTurretTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
    }
};

class basic_turret_lvl2 : public Turret
{
};

class basic_turret_lvl3 : public Turret
{
};

// -----------------------------------

/* LASER TURRET SYSTEMS
 * INVOLVES THREE TURRETS, IN ORDER OF UPGRADE
 * First turret : Fires a laser bolt, a fast moving projectile
 * Second Turret : Fires an instantaneous bolt from source to dest, damaging along the way
 * Third Turret : Fires a continuous sustained beam from source to dest for a few seconds, after which cools down. Damages everything in the path.
 */

class laser_turret_lvl1 : public Turret
{ // fires short beams of light
  public:
    laser_turret_lvl1(Vector2 pos, Tile &tile) : Turret(pos, tile, laser_bullet_speed, TurretType::LASER)
    {
        range = cyclone_turret_range;
        cooldownTimer = 1 / cyclone_turret_fire_rate;
        fireTimer = 0.0f; // initial timer, ready to fire
        rotationSpeed = 1.0f;
        m_recoilOffset = 8.0f;
    }
    void Draw() override
    {
        Vector2 baseOrigin = {(float)turretBaseTexture.width / 2.0f, (float)turretBaseTexture.height / 2.0f};

        DrawTexturePro(turretBaseTexture, {0, 0, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, {position.x, position.y, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, baseOrigin, 0.0f, WHITE);

        // calculating change in gun_rec to account for recoil

        float angleRad = gunRotation * DEG2RAD;
        Vector2 direction = {cosf(angleRad), sinf(angleRad)};
        Vector2 gunDrawPosition = Vector2Subtract(position, Vector2Scale(direction, recoilOffset));
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)scatterTurretTexture.width, (float)scatterTurretTexture.height};

        Vector2 gunOrigin = {(float)scatterTurretTexture.width / 2.0f, (float)scatterTurretTexture.height * 0.75f - 2.0f};
        DrawTexturePro(scatterTurretTexture, {0, 0, (float)scatterTurretTexture.width, (float)scatterTurretTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
    }
};

class laser_turret_lvl2 : public Turret
{
};

class laser_turret_lvl3 : public Turret
{
};
// ----------------------------------------

/* SLOWING TURRET SYSTEMS
 * Plan to be updated, only prototype here
 */

class slowing_turret_lvl1 : public Turret
{
  public:
    float active_timer, cooldown_timer;
    bool is_active = false;
    slowing_turret_lvl1(Vector2 pos, Tile &tile) : Turret(pos, tile, 0.0f, TurretType::SLOWING)
    {
        cooldown_timer = 0;
        this->active_timer = tsunami_turret_active_time;
        range = tsunami_turret_range;
    }
    void Update(float deltaTime) override
    {
        if (is_active)
        {
            active_timer -= deltaTime;
            if (active_timer <= 0)
            {
                is_active = false;
                cooldown_timer = tsunami_turret_cooldown_time;
            }
        }
        else
        {
            cooldown_timer -= deltaTime;
            if (cooldown_timer <= 0)
            {
                is_active = true;
                active_timer = tsunami_turret_active_time;
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

        DrawTexturePro(tsunamiTurretTX, {0, 0, (float)tsunamiTurretTX.width, (float)tsunamiTurretTX.height}, {position.x, position.y, (float)tsunamiTurretTX.width, (float)tsunamiTurretTX.height}, {(float)tsunamiTurretTX.width / 2, (float)tsunamiTurretTX.height / 2}, 0.0f, WHITE);

        if (is_active)
        {
            DrawCircleV(position, range, Fade(BLUE, 0.2f));
        }
    }
};

#pragma once
#include "Enemy.h"
#include "Entity.h"
#include "Map.h"
#include "Projectile.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include <Config.h>
#include <memory> // for std::unique_ptr
#include <vector>

enum class TurretType
{
    BASIC,
    LASER,
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
    void Update(float deltaTime, const std::vector<Enemy *> &targets, std::vector<std::unique_ptr<Entity>> &newProjectiles)
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
        basicTurretGunTexture = LoadTexture("assets/turrets/duo.png");

        // laser_turret
        laserTurretGunIMG = LoadImage("assets/turrets/cyclone.png");

        ImageResize(&laserTurretGunIMG, TILE_SIZE + 5.0f, TILE_SIZE + 5.0f);
        laserTurretGunTexture = LoadTextureFromImage(laserTurretGunIMG);
    }
    static void DestroyTextures()
    {
        // unload base
        UnloadTexture(turretBaseTexture);
        // unload basic_turret
        UnloadTexture(basicTurretGunTexture);
        // unload laser_turret
        UnloadTexture(laserTurretGunTexture);
    }

    //  protected:
    // the base of each turret. Common
    inline static Image turretBaseIMG;
    inline static Texture2D turretBaseTexture;

    // basic_turret (duo) stuff
    inline static Texture2D basicTurretGunTexture;

    // laser_turret (cyclone) stuff
    inline static Image laserTurretGunIMG;
    inline static Texture2D laserTurretGunTexture;

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

class basic_turret : public Turret
{
  public:
    basic_turret(Vector2 pos, Tile &tile) : Turret(pos, tile, normal_bullet_speed, TurretType::BASIC)
    {
        range = 3 * TILE_SIZE;
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
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)basicTurretGunTexture.width, (float)basicTurretGunTexture.height};

        Vector2 gunOrigin = {(float)basicTurretGunTexture.width / 2.0f, (float)basicTurretGunTexture.height * 0.75f - 2.0f};
        DrawTexturePro(basicTurretGunTexture, {0, 0, (float)basicTurretGunTexture.width, (float)basicTurretGunTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
    }
};

class laser_turret : public Turret
{ // fires short beams of light
  public:
    laser_turret(Vector2 pos, Tile &tile) : Turret(pos, tile, laser_bullet_speed, TurretType::LASER)
    {
        range = 7 * TILE_SIZE;
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
        //
        float angleRad = gunRotation * DEG2RAD;
        Vector2 direction = {cosf(angleRad), sinf(angleRad)};
        Vector2 gunDrawPosition = Vector2Subtract(position, Vector2Scale(direction, recoilOffset));
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)laserTurretGunTexture.width, (float)laserTurretGunTexture.height};

        Vector2 gunOrigin = {(float)laserTurretGunTexture.width / 2.0f, (float)laserTurretGunTexture.height * 0.75f - 2.0f};
        DrawTexturePro(laserTurretGunTexture, {0, 0, (float)laserTurretGunTexture.width, (float)laserTurretGunTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
    }
};

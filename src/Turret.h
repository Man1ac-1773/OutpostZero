#pragma once
#include "Enemy.h"
#include "Entity.h"
#include "Map.h"
#include "Particles.h"
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
    DUO,
    SCATTER,
    CYCLONE,
    MELTDOWN,
    WAVE,
    SALVO,
    TSUNAMI,
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
                case TurretType::SCATTER:
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

        duoTurretTexture = LoadTexture("assets/turrets/duo.png");

        scatterTurretIMG = LoadImage("assets/turrets/scatter.png");
        ImageResize(&scatterTurretIMG, TILE_SIZE, TILE_SIZE);
        scatterTurretTexture = LoadTextureFromImage(scatterTurretIMG);
        UnloadImage(scatterTurretIMG);

        cycloneTurretIMG = LoadImage("assets/turrets/cyclone.png");
        ImageResize(&cycloneTurretIMG, TILE_SIZE, TILE_SIZE);
        cycloneTurretTexture = LoadTextureFromImage(cycloneTurretIMG);
        UnloadImage(cycloneTurretIMG);

        meltdownTurretIMG = LoadImage("assets/turrets/meltdown.png");
        ImageResize(&meltdownTurretIMG, TILE_SIZE + 5.0f, TILE_SIZE + 5.0f);
        meltdownTurretTexture = LoadTextureFromImage(meltdownTurretIMG);
        UnloadImage(meltdownTurretIMG);

        waveTurretIMG = LoadImage("assets/turrets/wave.png");
        ImageResize(&waveTurretIMG, TILE_SIZE, TILE_SIZE);
        waveTurretTX = LoadTextureFromImage(waveTurretIMG);
        UnloadImage(waveTurretIMG);
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
    // meltdown
    inline static Image meltdownTurretIMG;
    inline static Texture2D meltdownTurretTexture;

    // slow turret (wave) stuff
    inline static Image waveTurretIMG;
    inline static Texture2D waveTurretTX;

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
    virtual float GetRotationSpeed() = 0;
};
// --------------------------------------------

/* BASIC TURRET SYSTEMS
 * THREE TURRETS, IN ORDER OF PROGRESSION
 */
class duo_turret : public Turret
{

  public:
    duo_turret(Vector2 pos, Tile &tile) : Turret(pos, tile, normal_bullet_speed, TurretType::DUO)
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
        //
        float angleRad = gunRotation * DEG2RAD;
        Vector2 direction = {cosf(angleRad), sinf(angleRad)};
        Vector2 gunDrawPosition = Vector2Subtract(position, Vector2Scale(direction, recoilOffset));
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)duoTurretTexture.width, (float)duoTurretTexture.height};

        Vector2 gunOrigin = {(float)duoTurretTexture.width / 2.0f, (float)duoTurretTexture.height * 0.75f - 2.0f};
        DrawTexturePro(duoTurretTexture, {0, 0, (float)duoTurretTexture.width, (float)duoTurretTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
    }

  private:
    float GetRotationSpeed() override { return rotationSpeed; }
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

class scatter_turret : public Turret
{ // fires short beams of light
  public:
    scatter_turret(Vector2 pos, Tile &tile) : Turret(pos, tile, scatter_bullet_speed, TurretType::SCATTER)
    {
        range = scatter_turret_range;
        cooldownTimer = 1 / scatter_turret_fire_rate;
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
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)scatterTurretTexture.width, (float)scatterTurretTexture.height};

        Vector2 gunOrigin = {(float)scatterTurretTexture.width / 2.0f, (float)scatterTurretTexture.height * 0.75f - 2.0f};
        DrawTexturePro(scatterTurretTexture, {0, 0, (float)scatterTurretTexture.width, (float)scatterTurretTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
    }

  private:
    float GetRotationSpeed() override { return rotationSpeed; }
};

class cyclone_turret : public Turret
{
  public:
    float cooldown_timer;
    float beam_timer;
    bool is_active;
    Vector2 target_pos;
    cyclone_turret(Vector2 pos, Tile &tile) : Turret(pos, tile, 0.0f, TurretType::CYCLONE)
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
                    for (auto &enemy : targets)
                    {
                        if (CheckCollisionCircleLine(enemy->GetPosition(), enemy->GetRadius(), position, target_pos))
                        {
                            enemy->TakeDamage(ProjectileType::CYCLONE_BEAM, 1.0f);
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

  private:
    float GetRotationSpeed() override { return rotationSpeed; }
};

class meltdown_turret : public Turret
{
  public:
    float cooldown_timer;
    float beam_timer;
    bool is_active;
    Vector2 target_pos;
    float recoil_velocity = 100.0f;
    float beamThickness = 10.0f;
    meltdown_turret(Vector2 pos, Tile &tile) : Turret(pos, tile, 0.0f, TurretType::MELTDOWN)
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
            recoilOffset = Lerp(recoilOffset, 0.0f, 1.5f * deltaTime);
        }
    }
    void Update(float deltaTime, const std::vector<Enemy *> &targets, std::vector<std::unique_ptr<Entity>> &newProjectiles) override
    {

        float min_dist = 99999.0f;
        Enemy *target_ptr = nullptr;
        for (auto &enemy : targets)
        {
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
        DrawTexturePro(turretBaseTexture, {0, 0, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, {position.x, position.y, (float)turretBaseTexture.width, (float)turretBaseTexture.height}, {(float)turretBaseTexture.width / 2.0f, (float)turretBaseTexture.height / 2.0f}, 0.0f, WHITE);

        if (is_active)
        {
            BeginBlendMode(BLEND_ADDITIVE);

            float lifeRatio = beam_timer / cyclone_turret_beam_timer;
            float alpha = lifeRatio * 0.8f;

            DrawLineEx(position, target_pos, beamThickness, Fade(ORANGE, alpha * 0.2f));
            DrawLineEx(position, target_pos, beamThickness * 0.8f, Fade(YELLOW, alpha * 0.5f));
            DrawLineEx(position, target_pos, beamThickness * 0.2f, Fade(WHITE, alpha));

            EndBlendMode();
        }
        float angleRad = gunRotation * DEG2RAD;
        Vector2 direction = {cosf(angleRad), sinf(angleRad)};
        Vector2 gunDrawPosition = Vector2Subtract(position, Vector2Scale(direction, recoilOffset));
        Rectangle gunDestRec = {gunDrawPosition.x, gunDrawPosition.y, (float)meltdownTurretTexture.width, (float)meltdownTurretTexture.height};

        Vector2 gunOrigin = {(float)meltdownTurretTexture.width / 2.0f, (float)meltdownTurretTexture.height * 0.75f - 2.0f};
        DrawTexturePro(meltdownTurretTexture, {0, 0, (float)meltdownTurretTexture.width, (float)meltdownTurretTexture.height}, // gun source rectangle
                       gunDestRec, gunOrigin, gunRotation + 90.0f, WHITE);
    }

  private:
    float GetRotationSpeed() override { return rotationSpeed; }
};
// ----------------------------------------

/* SLOWING TURRET SYSTEMS
 * Plan to be updated, only prototype here
 */

class wave_turret : public Turret
{
  public:
    float active_timer, cooldown_timer;
    bool is_active = false;
    wave_turret(Vector2 pos, Tile &tile) : Turret(pos, tile, 0.0f, TurretType::WAVE)
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

        DrawTexturePro(waveTurretTX, {0, 0, (float)waveTurretTX.width, (float)waveTurretTX.height}, {position.x, position.y, (float)waveTurretTX.width, (float)waveTurretTX.height}, {(float)waveTurretTX.width / 2, (float)waveTurretTX.height / 2}, 0.0f, WHITE);

        if (is_active)
        {
            DrawCircleV(position, range, Fade(BLUE, 0.2f));
        }
    }

  private:
    float GetRotationSpeed() override { return rotationSpeed; }
};

#pragma once
#include "Config.h"
#include "Entity.h"
#include "Particles.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include <cmath>
#include <iostream>
using namespace std;
enum class StatusEffects
{
    NONE,
    SLOWED,
    BURNING,
    STUNNED,

};
class Enemy : public Entity
{
  public:
    float radius;
    float speed;
    float original_speed;
    int counter = 0;
    float hp;
    bool took_damage = false;
    StatusEffects status_effect = StatusEffects::NONE;
    float status_timer = 0.0f;
    Vector2 targetPos = targets[counter];
    inline static int enemy_count = 0;
    Enemy()
    {
        cout << "Current startPos : {" << startPos.x << ", " << startPos.y << "}" << endl;
        position = startPos;
        enemy_count++;
    }
    void TakeDamage(ProjectileType proj_type)
    {
        switch (proj_type)
        {
        case ProjectileType::DUO_BASIC:
        {
            hp -= normal_bullet_damage;
            break;
        }
        case ProjectileType::LASER_BASIC:
        {
            hp -= laser_bullet_damage;
            break;
        }
        }
        if (hp <= 0)
        {
            Destroy();
            particles.SpawnExplosion(position, proj_type);
            // Death animation
            return;
        }
        took_damage = true;
    }

    float GetRadius() { return radius; }
    void Update(float deltaTime) override
    {

        if (status_effect != StatusEffects::NONE && status_timer <= 0)
        {
            switch (status_effect)
            {
            case StatusEffects::SLOWED:
            {
                this->speed = original_speed * 0.5f;
                status_timer = 5.0f;
                break;
            }
            }
        }

        if (status_timer > 0)
        {
            status_timer -= deltaTime;
            if (status_timer <= 0)
            {
                switch (status_effect)
                {
                case StatusEffects::SLOWED:
                {
                    speed = original_speed;
                    break;
                }
                }
                status_effect = StatusEffects::NONE;
                status_timer = 0.0f;
            }
        }
        if (position.y >= GRID_ROWS * TILE_SIZE)
        {
            Destroy();
        }
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
    }
    virtual void Draw() override = 0;
    void Update()
    {

        if (targetPos == targets.back())
        {
            targetPos.y += 40;
        }
        if (Vector2DistanceSqr(position, targetPos) < radius)
        {
            velocity = {0, 0};
            counter++;
            targetPos = targets[counter];
            return;
        }

        velocity = velFromSpeed(position, targetPos, speed);
    }

    void Destroy() override
    {
        is_active = false;
        enemy_count--;
    }

    static void LoadTextures()
    {
        standard_enemyIMG = LoadImage("assets/units/Flare.png");
        ImageResize(&standard_enemyIMG, 32, 32);
        standard_enemyTX = LoadTextureFromImage(standard_enemyIMG);
        UnloadImage(standard_enemyIMG);

        fast_enemyIMG = LoadImage("assets/units/Mono.png");
        ImageResize(&fast_enemyIMG, 24, 24);
        fast_enemyTX = LoadTextureFromImage(fast_enemyIMG);
    }
    static void DestroyTextures()
    {
        UnloadTexture(standard_enemyTX);
        UnloadTexture(fast_enemyTX);
    }

  protected:
    // enemy : flare
    // stats : starter, basic
    inline static Image standard_enemyIMG;
    inline static Texture2D standard_enemyTX;

    // enemy : mono
    // stats : fast, basic
    inline static Image fast_enemyIMG;
    inline static Texture2D fast_enemyTX;
};

class standard_enemy : public Enemy
{
  public:
    standard_enemy()
    {
        radius = standard_enemy_radius;
        speed = standard_enemy_speed;
        original_speed = speed;
        hp = standard_enemy_health;
        velocity = velFromSpeed(position, targets[counter], speed);
    }
    void Draw() override
    {
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;
        if (!took_damage)
        {
            DrawTexturePro(standard_enemyTX, {0, 0, (float)standard_enemyTX.width, (float)standard_enemyTX.height}, {position.x, position.y, (float)standard_enemyTX.width, (float)standard_enemyTX.height}, {standard_enemyTX.width / 2.0f, standard_enemyTX.height / 2.0f}, rotation, WHITE);
        }
        else
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(standard_enemyTX, {0, 0, (float)standard_enemyTX.width, (float)standard_enemyTX.height}, {position.x, position.y, (float)standard_enemyTX.width, (float)standard_enemyTX.height}, {standard_enemyTX.width / 2.0f, standard_enemyTX.height / 2.0f}, rotation, WHITE);
            took_damage = false;
            EndBlendMode();
        }
    }
};

class fast_enemy : public Enemy
{
  public:
    fast_enemy()
    {
        radius = fast_enemy_radius;
        speed = fast_enemy_speed;
        original_speed = speed;
        hp = fast_enemy_health;
        velocity = velFromSpeed(position, targetPos, speed);
    }

    void Draw() override
    {
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;
        DrawTexturePro(fast_enemyTX, {0, 0, (float)fast_enemyTX.width, (float)fast_enemyTX.height}, {position.x, position.y, (float)fast_enemyTX.width, (float)fast_enemyTX.height}, {fast_enemyTX.width / 2.0f, fast_enemyTX.height / 2.0f}, rotation, WHITE);
    }
};

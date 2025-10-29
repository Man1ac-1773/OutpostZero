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
enum class EnemyType
{
    FLARE,
    MONO,
    CRAWLER,
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
    bool isVisible = true;
    StatusEffects status_effect = StatusEffects::NONE;
    float status_timer = 0.0f;
    Vector2 targetPos = targets[counter];
    inline static int enemy_count = 0;
    Enemy()
    {
        position = startPos;
        enemy_count++;
    }
    // idea to use multiplier as a fall-off for distance
    void TakeDamage(ProjectileType proj_type, float multiplier)
    {
        switch (proj_type)
        {
        case ProjectileType::DUO_BASIC:
        {
            hp -= normal_bullet_damage * multiplier;
            break;
        }
        case ProjectileType::FLAME:
        {
            hp -= flame_bullet_damage * multiplier;
            break;
        }
        case ProjectileType::LASER:
        {
            hp -= scatter_bullet_damage * multiplier;
            break;
        }
        case ProjectileType::CYCLONE_BEAM:
        {
            hp -= cyclone_beam_damage * multiplier;
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
    void TakeDamageByValue(ProjectileType proj_type, float amount)
    {
        hp -= amount;
        if (hp <= 0)
        {
            Destroy();
            particles.SpawnExplosion(position, proj_type);
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

                if (GetEnemyType() == EnemyType::CRAWLER)
                {
                    this->speed = original_speed * 0.3f;
                    status_timer = 2.0f;
                    isVisible = true;
                    break;
                }

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
                    if (GetEnemyType() == EnemyType::CRAWLER)
                    {
                        isVisible = false;
                    }
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
            player_health--;
        }
        if (position.x >= GRID_COLS * TILE_SIZE || position.y == NAN || position.x == NAN)
        {
            Destroy();
        }
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
    }
    virtual void Draw() override = 0;
    virtual EnemyType GetEnemyType() = 0;
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
        if (is_active)
        {
            is_active = false;
            enemy_count--;
        }
        return;
    }

    static void LoadTextures()
    {
        flare_enemyIMG = LoadImage("assets/units/Flare.png");
        ImageResize(&flare_enemyIMG, 32, 32);
        flare_enemyTX = LoadTextureFromImage(flare_enemyIMG);
        UnloadImage(flare_enemyIMG);

        mono_enemyIMG = LoadImage("assets/units/Mono.png");
        ImageResize(&mono_enemyIMG, 24, 24);
        mono_enemyTX = LoadTextureFromImage(mono_enemyIMG);
        UnloadImage(mono_enemyIMG);

        crawler_enemyIMG = LoadImage("assets/units/Crawler.png");
        ImageResize(&crawler_enemyIMG, 24, 24);
        crawler_enemyTX = LoadTextureFromImage(crawler_enemyIMG);
        UnloadImage(crawler_enemyIMG);

        heartIMG = LoadImage("assets/others/heart.png");
        heartTX = LoadTextureFromImage(heartIMG);
        UnloadImage(heartIMG);
    }
    static void DestroyTextures()
    {
        UnloadTexture(flare_enemyTX);
        UnloadTexture(mono_enemyTX);
        UnloadTexture(heartTX);
    }

    // heart for player
    inline static Image heartIMG;
    inline static Texture2D heartTX;

  protected:
    // enemy : flare
    inline static Image flare_enemyIMG;
    inline static Texture2D flare_enemyTX;

    // enemy : mono
    inline static Image mono_enemyIMG;
    inline static Texture2D mono_enemyTX;

    // enemy : crawler
    inline static Image crawler_enemyIMG;
    inline static Texture2D crawler_enemyTX;
};

class flare_enemy : public Enemy
{
  public:
    flare_enemy()
    {
        radius = flare_enemy_radius;
        speed = flare_enemy_speed;
        original_speed = speed;
        hp = flare_enemy_health;
        // this one line caused a bug that took me 4 hours to find and fix. I hate u
        velocity = velFromSpeed(position, targets[counter], speed);
    }
    void Draw() override
    {
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;
        if (!took_damage)
        {
            DrawTexturePro(flare_enemyTX, {0, 0, (float)flare_enemyTX.width, (float)flare_enemyTX.height}, {position.x, position.y, (float)flare_enemyTX.width, (float)flare_enemyTX.height}, {flare_enemyTX.width / 2.0f, flare_enemyTX.height / 2.0f}, rotation, WHITE);
        }
        else
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(flare_enemyTX, {0, 0, (float)flare_enemyTX.width, (float)flare_enemyTX.height}, {position.x, position.y, (float)flare_enemyTX.width, (float)flare_enemyTX.height}, {flare_enemyTX.width / 2.0f, flare_enemyTX.height / 2.0f}, rotation, WHITE);
            took_damage = false;
            EndBlendMode();
        }
        DrawHealthBar(hp, flare_enemy_health, position);
    }
    EnemyType GetEnemyType() override { return EnemyType::FLARE; }
};

class mono_enemy : public Enemy
{
  public:
    mono_enemy()
    {
        radius = mono_enemy_radius;
        speed = mono_enemy_speed;
        original_speed = speed;
        hp = mono_enemy_health;
        velocity = velFromSpeed(position, targetPos, speed);
    }

    void Draw() override
    {
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;
        if (!took_damage)
        {
            DrawTexturePro(mono_enemyTX, {0, 0, (float)mono_enemyTX.width, (float)mono_enemyTX.height}, {position.x, position.y, (float)mono_enemyTX.width, (float)mono_enemyTX.height}, {mono_enemyTX.width / 2.0f, mono_enemyTX.height / 2.0f}, rotation, WHITE);
        }
        else
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(mono_enemyTX, {0, 0, (float)mono_enemyTX.width, (float)mono_enemyTX.height}, {position.x, position.y, (float)mono_enemyTX.width, (float)mono_enemyTX.height}, {mono_enemyTX.width / 2.0f, mono_enemyTX.height / 2.0f}, rotation, WHITE);
            EndBlendMode();
        }
        DrawHealthBar(hp, mono_enemy_health, position);
    }
    EnemyType GetEnemyType() override { return EnemyType::MONO; }
};

class crawler_enemy : public Enemy
{
  public:
    crawler_enemy()
    {
        radius = crawler_enemy_radius;
        speed = crawler_enemy_speed;
        hp = crawler_enemy_health;
        original_speed = speed;
        isVisible = false;
        velocity = velFromSpeed(position, targetPos, speed);
    }
    void Draw() override
    {
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;
        if (isVisible)
        {
            DrawTexturePro(crawler_enemyTX, {0, 0, (float)crawler_enemyTX.width, (float)crawler_enemyTX.height}, {position.x, position.y, (float)crawler_enemyTX.width, (float)crawler_enemyTX.height}, {crawler_enemyTX.width / 2.0f, crawler_enemyTX.height / 2.0f}, rotation, WHITE);
        }
        else
        {
            DrawTexturePro(crawler_enemyTX, {0, 0, (float)crawler_enemyTX.width, (float)crawler_enemyTX.height}, {position.x, position.y, (float)crawler_enemyTX.width, (float)crawler_enemyTX.height}, {crawler_enemyTX.width / 2.0f, crawler_enemyTX.height / 2.0f}, rotation, Fade(WHITE, 0.2f));
        }
    }
    EnemyType GetEnemyType() override { return EnemyType::CRAWLER; }
};

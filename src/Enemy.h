#pragma once
#include "Config.h"
#include "Entity.h"
#include "Particles.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include <algorithm>
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
    POLY,
    LOCUS,
    ANTUMBRA,
};
class Enemy : public Entity
{
  public:
    float radius;
    float speed;
    float original_speed;
    int map_counter = 0;
    unsigned long long id;
    float hp;
    float max_hp;
    bool took_damage = false;
    bool isVisible = true;
    bool healed_this_frame = false;
    int kill_reward;
    StatusEffects status_effect = StatusEffects::NONE;
    float status_timer = 0.0f;
    Vector2 targetPos = targets[map_counter];
    Enemy()
    {
        id = next_id++;
        position = startPos;
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
        case ProjectileType::SMITE:
        {
            hp -= shotgun_bullet_damage * multiplier;
            break;
        }
        case ProjectileType::LASER:
        {
            hp -= lancer_bullet_damage * multiplier;
            break;
        }
        case ProjectileType::CYCLONE_BEAM:
        {
            hp -= cyclone_beam_damage * multiplier;
            break;
        }
        case ProjectileType::ICE_STREAM:
        {
            hp -= ice_stream_damage * multiplier;
            if (status_effect != StatusEffects::SLOWED)
            {
                status_effect = StatusEffects::SLOWED;
            }
            break;
        }
        }
        if (hp <= 0)
        {
            playerMoney += kill_reward;
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
            playerMoney += kill_reward;
            particles.SpawnExplosion(position, proj_type);
        }
        took_damage = true;
    }
    float GetRadius() { return radius; }
    // A simple place-holder function IN-CASE some enemies do actions
    // Don't want to put it in the update function, that handles too many things
    // Most enemies migth just have this function empty, and some have something
    // to do in here
    // Update : poly_enemy actually does something
    virtual void DoEnemyAction(vector<Enemy *> &targets, float deltaTime) {}

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
            // the NAN check exists only to catch whenever a variable remains uninitialised and crashes the whole thing
            // Happened a few times and debugging was a nightmare
            // Realised I can't guarantee existence especially when rolling out new features.
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
            map_counter++;
            targetPos = targets[map_counter];
            return;
        }
        velocity = velFromSpeed(position, targetPos, speed);
    }

    void Destroy() override
    {
        if (is_active)
        {
            is_active = false;
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

        poly_enemyIMG = LoadImage("assets/units/Poly.png");
        ImageResize(&poly_enemyIMG, 32, 32);
        poly_enemyTX = LoadTextureFromImage(poly_enemyIMG);
        UnloadImage(poly_enemyIMG);

        locus_enemyIMG = LoadImage("assets/units/Locus.png");
        ImageResize(&locus_enemyIMG, 40, 40);
        locus_enemyTX = LoadTextureFromImage(locus_enemyIMG);
        UnloadImage(locus_enemyIMG);

        antumbra_enemyIMG = LoadImage("assets/units/Antumbra.png");
        ImageResize(&antumbra_enemyIMG, 64, 64);
        antumbra_enemyTX = LoadTextureFromImage(antumbra_enemyIMG);
        UnloadImage(antumbra_enemyIMG);

        heartIMG = LoadImage("assets/others/heart.png");
        heartTX = LoadTextureFromImage(heartIMG);
        UnloadImage(heartIMG);

        currencyIMG = LoadImage("assets/others/currency.png");
        ImageResize(&currencyIMG, 20, 20);
        currencyTX = LoadTextureFromImage(currencyIMG);
        UnloadImage(currencyIMG);
    }
    static void DestroyTextures()
    {
        UnloadTexture(flare_enemyTX);
        UnloadTexture(mono_enemyTX);
        UnloadTexture(heartTX);
    }

    /* a lot of declarations here huh
     * Need explained in Turret.h
     * Or read documentation
     */
    // heart for player
    inline static Image heartIMG;
    inline static Texture2D heartTX;

    // currency
    inline static Image currencyIMG;
    inline static Texture2D currencyTX;

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

    // enemy : poly
    inline static Image poly_enemyIMG;
    inline static Texture2D poly_enemyTX;

    // locus enemy
    inline static Image locus_enemyIMG;
    inline static Texture2D locus_enemyTX;

    // antumbra boss
    inline static Image antumbra_enemyIMG;
    inline static Texture2D antumbra_enemyTX;

  private:
    static inline unsigned long long next_id = 0;
};

class flare_enemy : public Enemy
{
  public:
    flare_enemy()
    {
        radius = flare_enemy_radius;
        speed = flare_enemy_speed;
        original_speed = speed;
        hp = flare_enemy_health * enemy_health_multiplier;
        max_hp = flare_enemy_health * enemy_health_multiplier;
        kill_reward = flare_enemy_reward;
        // this one line caused a bug that took me 4 hours to find and fix. I hate u
        velocity = velFromSpeed(position, targetPos, speed);
    }
    void Draw() override
    {
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;
        if (took_damage)
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(flare_enemyTX, {0, 0, (float)flare_enemyTX.width, (float)flare_enemyTX.height}, {position.x, position.y, (float)flare_enemyTX.width, (float)flare_enemyTX.height}, {flare_enemyTX.width / 2.0f, flare_enemyTX.height / 2.0f}, rotation, WHITE);
            took_damage = false;
            EndBlendMode();
        }
        else if (healed_this_frame)
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(flare_enemyTX, {0, 0, (float)flare_enemyTX.width, (float)flare_enemyTX.height}, {position.x, position.y, (float)flare_enemyTX.width, (float)flare_enemyTX.height}, {flare_enemyTX.width / 2.0f, flare_enemyTX.height / 2.0f}, rotation, GREEN);
            EndBlendMode();
            healed_this_frame = false;
        }
        else if (status_effect == StatusEffects::SLOWED)
        {
            DrawTexturePro(flare_enemyTX, {0, 0, (float)flare_enemyTX.width, (float)flare_enemyTX.height}, {position.x, position.y, (float)flare_enemyTX.width, (float)flare_enemyTX.height}, {flare_enemyTX.width / 2.0f, flare_enemyTX.height / 2.0f}, rotation, SKYBLUE);
        }
        else
        {
            DrawTexturePro(flare_enemyTX, {0, 0, (float)flare_enemyTX.width, (float)flare_enemyTX.height}, {position.x, position.y, (float)flare_enemyTX.width, (float)flare_enemyTX.height}, {flare_enemyTX.width / 2.0f, flare_enemyTX.height / 2.0f}, rotation, WHITE);
        }
        DrawHealthBar(hp, max_hp, position);
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
        hp = mono_enemy_health * enemy_health_multiplier;
        max_hp = mono_enemy_health * enemy_health_multiplier;
        kill_reward = mono_enemy_reward;
        velocity = velFromSpeed(position, targetPos, speed);
    }

    void Draw() override
    {
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;
        if (took_damage)
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(mono_enemyTX, {0, 0, (float)mono_enemyTX.width, (float)mono_enemyTX.height}, {position.x, position.y, (float)mono_enemyTX.width, (float)mono_enemyTX.height}, {mono_enemyTX.width / 2.0f, mono_enemyTX.height / 2.0f}, rotation, WHITE);
            EndBlendMode();
            took_damage = false;
        }
        else if (healed_this_frame)
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(mono_enemyTX, {0, 0, (float)mono_enemyTX.width, (float)mono_enemyTX.height}, {position.x, position.y, (float)mono_enemyTX.width, (float)mono_enemyTX.height}, {mono_enemyTX.width / 2.0f, mono_enemyTX.height / 2.0f}, rotation, GREEN);
            EndBlendMode();
            healed_this_frame = false;
        }
        else if (status_effect == StatusEffects::SLOWED)
        {
            DrawTexturePro(mono_enemyTX, {0, 0, (float)mono_enemyTX.width, (float)mono_enemyTX.height}, {position.x, position.y, (float)mono_enemyTX.width, (float)mono_enemyTX.height}, {mono_enemyTX.width / 2.0f, mono_enemyTX.height / 2.0f}, rotation, SKYBLUE);
        }
        else
        {
            DrawTexturePro(mono_enemyTX, {0, 0, (float)mono_enemyTX.width, (float)mono_enemyTX.height}, {position.x, position.y, (float)mono_enemyTX.width, (float)mono_enemyTX.height}, {mono_enemyTX.width / 2.0f, mono_enemyTX.height / 2.0f}, rotation, WHITE);
        }
        DrawHealthBar(hp, max_hp, position);
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
        hp = crawler_enemy_health * enemy_health_multiplier;
        max_hp = crawler_enemy_health * enemy_health_multiplier;
        original_speed = speed;
        kill_reward = crawler_enemy_reward;
        isVisible = false;

        velocity = velFromSpeed(position, targetPos, speed);
    }
    void Draw() override
    {
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;
        if (isVisible)
        {
            DrawTexturePro(crawler_enemyTX, {0, 0, (float)crawler_enemyTX.width, (float)crawler_enemyTX.height}, {position.x, position.y, (float)crawler_enemyTX.width, (float)crawler_enemyTX.height}, {crawler_enemyTX.width / 2.0f, crawler_enemyTX.height / 2.0f}, rotation, WHITE);
            DrawHealthBar(hp, max_hp, position);
        }
        else if (took_damage) // flash white for one frame
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(crawler_enemyTX, {0, 0, (float)crawler_enemyTX.width, (float)crawler_enemyTX.height}, {position.x, position.y, (float)crawler_enemyTX.width, (float)crawler_enemyTX.height}, {crawler_enemyTX.width / 2.0f, crawler_enemyTX.height / 2.0f}, rotation, WHITE);
            EndBlendMode();
            took_damage = false;
        }
        else if (healed_this_frame)
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(crawler_enemyTX, {0, 0, (float)crawler_enemyTX.width, (float)crawler_enemyTX.height}, {position.x, position.y, (float)crawler_enemyTX.width, (float)crawler_enemyTX.height}, {crawler_enemyTX.width / 2.0f, crawler_enemyTX.height / 2.0f}, rotation, GREEN);
            EndBlendMode();
            healed_this_frame = false;
        }
        else // faded view, appears psuedo-invisible
        {
            DrawTexturePro(crawler_enemyTX, {0, 0, (float)crawler_enemyTX.width, (float)crawler_enemyTX.height}, {position.x, position.y, (float)crawler_enemyTX.width, (float)crawler_enemyTX.height}, {crawler_enemyTX.width / 2.0f, crawler_enemyTX.height / 2.0f}, rotation, Fade(WHITE, 0.2f));
        }
    }
    EnemyType GetEnemyType() override { return EnemyType::CRAWLER; }
};

/* Implementation of an enemy with an actual role
 * HEALER ENEMY
 * Loops through all enemies within it's range and updates their health.
 * Itself slow and weak
 */
class poly_enemy : public Enemy
{
  public:
    float range;
    float heal_cooldown;
    poly_enemy()
    {
        radius = poly_enemy_radius;
        speed = poly_enemy_speed;
        hp = poly_enemy_health * enemy_health_multiplier;
        max_hp = poly_enemy_health * enemy_health_multiplier;
        original_speed = speed;
        kill_reward = poly_enemy_reward;
        velocity = velFromSpeed(position, targetPos, speed);
        range = 3.0f * TILE_SIZE;
    }

    void Draw() override
    {
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;

        if (took_damage) // flash white for one frame
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(poly_enemyTX, {0, 0, (float)poly_enemyTX.width, (float)poly_enemyTX.height}, {position.x, position.y, (float)poly_enemyTX.width, (float)poly_enemyTX.height}, {poly_enemyTX.width / 2.0f, poly_enemyTX.height / 2.0f}, rotation, WHITE);
            EndBlendMode();
            took_damage = false;
        }

        else if (status_effect == StatusEffects::SLOWED) // skyblue faded color
        {
            DrawTexturePro(poly_enemyTX, {0, 0, (float)poly_enemyTX.width, (float)poly_enemyTX.height}, {position.x, position.y, (float)poly_enemyTX.width, (float)poly_enemyTX.height}, {poly_enemyTX.width / 2.0f, poly_enemyTX.height / 2.0f}, rotation, SKYBLUE);
        }
        else
        {
            DrawTexturePro(poly_enemyTX, {0, 0, (float)poly_enemyTX.width, (float)poly_enemyTX.height}, {position.x, position.y, (float)poly_enemyTX.width, (float)poly_enemyTX.height}, {poly_enemyTX.width / 2.0f, poly_enemyTX.height / 2.0f}, rotation, WHITE);
        }
        DrawHealthBar(hp, max_hp, position);
    }
    void DoEnemyAction(vector<Enemy *> &targets, float deltaTime) override
    {
        if (heal_cooldown > 0)
        {
            heal_cooldown -= deltaTime;
            return;
        }
        for (auto &enemy : targets)
        {
            if (enemy->GetEnemyType() != EnemyType::POLY && Vector2DistanceSqr(position, enemy->GetPosition()) <= range * range && enemy->hp < enemy->max_hp)
            {
                // note that we don't care what type of target it is, just heal anything but itself
                // we also don't care if the target has been healed this frame or not, because
                // multiple healers can heal the same target in one frame
                enemy->healed_this_frame = true;
                enemy->hp += poly_enemy_heal_amount; 
                // this flag is un-set only in the draw loop,
                // which is why the detection and action happen here
                // itself, and not seperately.
            }
        }
        heal_cooldown = max_heal_cooldown;
        // cooldown only after healing everyone possible
        // always tries to heal everyone if cooldown is allowing it
    }
    EnemyType GetEnemyType() override { return EnemyType::POLY; }
};

/* Tank like enemy
 */
class locus_enemy : public Enemy
{
  public:
    locus_enemy()
    {
        radius = locus_enemy_radius;
        speed = locus_enemy_speed;
        original_speed = speed;
        hp = locus_enemy_health * enemy_health_multiplier;
        max_hp = locus_enemy_health * enemy_health_multiplier;
        kill_reward = locus_enemy_reward;
        velocity = velFromSpeed(position, targetPos, speed);
    }
    void Draw() override
    {
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;
        if (took_damage)
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(locus_enemyTX, {0, 0, (float)locus_enemyTX.width, (float)locus_enemyTX.height}, {position.x, position.y, (float)locus_enemyTX.width, (float)locus_enemyTX.height}, {locus_enemyTX.width / 2.0f, locus_enemyTX.height / 2.0f}, rotation, WHITE);
            EndBlendMode();
            took_damage = false;
        }
        else if (healed_this_frame)
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(locus_enemyTX, {0, 0, (float)locus_enemyTX.width, (float)locus_enemyTX.height}, {position.x, position.y, (float)locus_enemyTX.width, (float)locus_enemyTX.height}, {locus_enemyTX.width / 2.0f, locus_enemyTX.height / 2.0f}, rotation, GREEN);
            EndBlendMode();
            healed_this_frame = false;
        }
        else if (status_effect == StatusEffects::SLOWED)
        {
            DrawTexturePro(locus_enemyTX, {0, 0, (float)locus_enemyTX.width, (float)locus_enemyTX.height}, {position.x, position.y, (float)locus_enemyTX.width, (float)locus_enemyTX.height}, {locus_enemyTX.width / 2.0f, locus_enemyTX.height / 2.0f}, rotation, SKYBLUE);
        }
        else
        {
            DrawTexturePro(locus_enemyTX, {0, 0, (float)locus_enemyTX.width, (float)locus_enemyTX.height}, {position.x, position.y, (float)locus_enemyTX.width, (float)locus_enemyTX.height}, {locus_enemyTX.width / 2.0f, locus_enemyTX.height / 2.0f}, rotation, WHITE);
        }
        DrawHealthBar(hp, max_hp, position);
    }
    EnemyType GetEnemyType() override { return EnemyType::LOCUS; }
};

class antumbra_enemy : public Enemy
{
  public:
    antumbra_enemy()
    {
        radius = antumbra_enemy_radius;
        speed = antumbra_enemy_speed;
        original_speed = speed;
        hp = antumbra_enemy_health * enemy_health_multiplier;
        max_hp = antumbra_enemy_health * enemy_health_multiplier;
        kill_reward = antumbra_enemy_reward;
        velocity = velFromSpeed(position, targetPos, speed);
    }
    void Draw() override
    {
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;
        if (took_damage)
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(antumbra_enemyTX, {0, 0, (float)antumbra_enemyTX.width, (float)antumbra_enemyTX.height}, {position.x, position.y, (float)antumbra_enemyTX.width, (float)antumbra_enemyTX.height}, {antumbra_enemyTX.width / 2.0f, antumbra_enemyTX.height / 2.0f}, rotation, WHITE);
            EndBlendMode();
            took_damage = false;
        }
        else if (healed_this_frame)
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(antumbra_enemyTX, {0, 0, (float)antumbra_enemyTX.width, (float)antumbra_enemyTX.height}, {position.x, position.y, (float)antumbra_enemyTX.width, (float)antumbra_enemyTX.height}, {antumbra_enemyTX.width / 2.0f, antumbra_enemyTX.height / 2.0f}, rotation, GREEN);
            EndBlendMode();
            healed_this_frame = false;
        }
        else if (status_effect == StatusEffects::SLOWED)
        {
            DrawTexturePro(antumbra_enemyTX, {0, 0, (float)antumbra_enemyTX.width, (float)antumbra_enemyTX.height}, {position.x, position.y, (float)antumbra_enemyTX.width, (float)antumbra_enemyTX.height}, {antumbra_enemyTX.width / 2.0f, antumbra_enemyTX.height / 2.0f}, rotation, SKYBLUE);
        }
        else
        {
            DrawTexturePro(antumbra_enemyTX, {0, 0, (float)antumbra_enemyTX.width, (float)antumbra_enemyTX.height}, {position.x, position.y, (float)antumbra_enemyTX.width, (float)antumbra_enemyTX.height}, {antumbra_enemyTX.width / 2.0f, antumbra_enemyTX.height / 2.0f}, rotation, WHITE);
        }
        DrawHealthBar(hp, max_hp, position);
    }
    EnemyType GetEnemyType() override { return EnemyType::ANTUMBRA; }
};

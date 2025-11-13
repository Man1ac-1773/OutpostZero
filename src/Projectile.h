#pragma once
#include "Config.h"
#include "Entity.h"
#include "Particles.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include <cmath>

class Projectile : public Entity
{
  public:
    float radius;
    Color color;
    bool active;
    Vector2 start_pos;
    Vector2 target_pos;
    int pierce_count;
    std::unordered_set<int> current_colliding;
    unsigned long long id;
    int enemies_hit = 0;
    Projectile(Vector2 startPos, Vector2 targetPos)
    {
        id = next_id++;
        start_pos = startPos;
        target_pos = targetPos;
        position = startPos; // starting position
        color = RED;         // color of the projectile
        active = true;       // state of the projectile
    }
    virtual ProjectileType getProjType() = 0;
    virtual float GetMaxProjRange() = 0;
    float GetRadius() const { return radius; }
    void ReducePierceCount()
    {
        pierce_count--;
        enemies_hit++;
        if (pierce_count <= 0)
        {
            Destroy();
        }
    }
    void Update(float deltaTime) override
    {
        // delete projectile if too far from firing pos;
        if (Vector2DistanceSqr(position, start_pos) > GetMaxProjRange())
        {
            Destroy();
            return;
        }
        position += Vector2Scale(velocity, deltaTime);
        particles.SpawnTrail(position, velocity, getProjType());
    }
    void Draw() override
    {
        if (active)
        {
            DrawCircleV(position, radius, color);
        }
    }
    static void LoadTextures()
    {
        BulletImage = LoadImage("assets/projectiles/smallBullet.png");
        ImageResize(&BulletImage, 10, 10);
        BulletTexture = LoadTextureFromImage(BulletImage);
        UnloadImage(BulletImage);

        flameIMG = LoadImage("assets/projectiles/flame.png");
        ImageResize(&flameIMG, 10, 10);
        flameTX = LoadTextureFromImage(flameIMG);
        UnloadImage(flameIMG);
        // laser
        LaserImage = LoadImage("assets/projectiles/laserBullet.png");
        ImageResize(&LaserImage, 40, 2);
        LaserTexture = LoadTextureFromImage(LaserImage);
        UnloadImage(LaserImage);

        iceIMG = LoadImage("assets/projectiles/ice_bullet.png");
        ImageResize(&iceIMG, 30, 30);
        iceTexture = LoadTextureFromImage(iceIMG);
        UnloadImage(iceIMG);
    }
    static void DestroyTextures()
    {
        UnloadTexture(BulletTexture);
        UnloadTexture(LaserTexture);
    }

  protected:
    // normal bullet
    inline static Image BulletImage;
    inline static Texture2D BulletTexture;

    inline static Image flameIMG;
    inline static Texture2D flameTX;
    // laser bullet
    inline static Image LaserImage;
    inline static Texture2D LaserTexture;

    inline static Image iceIMG;
    inline static Texture2D iceTexture;

  private:
    static inline unsigned long long next_id;
};

/* The basic projectile
 * Fired by duo_turret in a straight line
 */
class normal_bullet : public Projectile
{
  public:
    normal_bullet(Vector2 startPos, Vector2 targetPos) : Projectile(startPos, targetPos)
    {

        velocity = velFromSpeed(startPos, targetPos, normal_bullet_speed); // set velocity towards target
        radius = 3.0f;
        pierce_count = 1;
    }

    void Draw() override
    {
        float rotation = atan2f(velocity.x, velocity.y) * RAD2DEG;
        DrawTexturePro(BulletTexture, {0, 0, (float)BulletTexture.width, (float)BulletTexture.height}, {position.x, position.y, (float)BulletTexture.width, (float)BulletTexture.height}, {(float)BulletTexture.width / 2, (float)BulletTexture.height / 2}, rotation, WHITE);
    }
    ProjectileType getProjType() override { return ProjectileType::DUO_BASIC; }
    float GetMaxProjRange() override { return 1 << 20; }
};
/* The laser projectile
 * Fired by lancer_turret, spawns at the mouth of the gun then travels forward
 */
class laser_bullet : public Projectile
{
  public:
    laser_bullet(Vector2 startPos, Vector2 targetPos) : Projectile(startPos, targetPos)
    {
        velocity = velFromSpeed(startPos, targetPos, lancer_bullet_speed);
        spawnTimer = m_spawnTimer;
        state = ProjectileState::SPAWNING;
        radius = 1.5f;
        pierce_count = 2;
    }
    void Update(float deltaTime) override
    {
        // delete projectile if too far from firing pos;
        if (Vector2DistanceSqr(position, start_pos) > 1 << 20)
        {
            Destroy();
            return;
        }

        switch (state)
        {
        case ProjectileState::SPAWNING:
        {
            spawnTimer -= deltaTime;
            if (spawnTimer <= 0)
            {
                state = ProjectileState::FLYING;
            }
            break;
        }
        case ProjectileState::FLYING:
        {
            position += Vector2Scale(velocity, deltaTime);
            break;
        }
        }
    }

    void Draw() override
    {
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG;
        float toRender; // how much of the texture to draw;
        if (state == ProjectileState::SPAWNING)
        {
            toRender = (m_spawnTimer - spawnTimer) / m_spawnTimer;
        }
        else
        {
            toRender = 1.0f;
        }
        Rectangle sourceRec = {0, 0, (float)LaserTexture.width * toRender, (float)LaserTexture.height};
        Rectangle destRec = {position.x, position.y, (float)LaserTexture.width * toRender, (float)LaserTexture.height};

        Vector2 origin = {0, (float)LaserTexture.height / 2};

        BeginBlendMode(BLEND_ADDITIVE);
        DrawTexturePro(LaserTexture, sourceRec, destRec, origin, rotation, WHITE);
        EndBlendMode();
    }

    ProjectileType getProjType() override { return ProjectileType::LASER; }
    float GetMaxProjRange() override { return 1 << 20; }

  private:
    // parameters of feature only for this bullet type
    // This bullet spawns at the mouth of the gun before travelling like a normal  bullet
    // the spawnTimer is calibrated perfectly to reflect the travel speed and length of projectile,
    // so it seems like there is not discontinuity in bullet travel time.
    inline static float m_spawnTimer = 0.04f;
    float spawnTimer;
    enum class ProjectileState
    {
        SPAWNING,
        FLYING,
    };
    ProjectileState state;
};

/* The flame projectile
 * Fired by flame_turret in a straight line with some spread
 */

class flame_bullet : public Projectile
{
  public:
    float speed;
    float spreadAngle = 10.0f;
    float max_life;
    float life;
    flame_bullet(Vector2 startPos, Vector2 targetPos) : Projectile(startPos, targetPos)
    {
        // changing speed by +- 10% to sell the flame effect
        speed = flame_bullet_speed * (1 - (GetRandomValue(-10, 10) / 100.0f));
        // causing slight jitter in direction of travel to
        // make it seem like an actual flamethrower
        Vector2 dir = Vector2Normalize(targetPos - startPos);
        float spread = GetRandomValue(-spreadAngle, spreadAngle) * DEG2RAD;
        dir = Vector2Rotate(dir, spread);
        velocity = Vector2Scale(dir, speed);
        // for drawing fade
        max_life = (ripple_turret_range / speed) * 3.0f;
        life = max_life;
        pierce_count = 1;
        radius = 5.0f; // slightly bigger than projectile texture to give effect of flame
    }
    void Update(float deltaTime) override
    {
        // delete projectile if too far from firing pos;
        if (Vector2DistanceSqr(position, start_pos) > GetMaxProjRange())
        {
            Destroy();
            return;
        }
        life -= deltaTime;
        position += Vector2Scale(velocity, deltaTime);
    }

    void Draw() override
    {
        float rotation = atan2f(velocity.x, velocity.y) * RAD2DEG;
        float alpha = life / max_life;
        DrawTexturePro(flameTX, {0, 0, (float)flameTX.width, (float)flameTX.height}, {position.x, position.y, (float)flameTX.width, (float)flameTX.height}, {(float)flameTX.width / 2, (float)flameTX.height / 2}, rotation, Fade(WHITE, alpha));
    }
    // getter functions for parent class logic that implement something
    // based on the type/value from child classes
    ProjectileType getProjType() override { return ProjectileType::FLAME; }
    float GetMaxProjRange() override { return ripple_turret_range * ripple_turret_range; }
};

/* Ice bullet 
 * Fired by the salvo turret in a small spread
*/
class ice_bullet : public Projectile
{
  public:
    float speed;
    float spreadAngle = 5.0f;
    float max_life;
    float life;
    ice_bullet(Vector2 startPos, Vector2 targetPos) : Projectile(startPos, targetPos)
    {
        // changing speed by +- 10% to sell the flame effect
        speed = ice_stream_speed * (1 - (GetRandomValue(-10, 10) / 100.0f));
        // causing slight jitter in direction of travel to
        // make it seem like an actual flamethrower
        Vector2 dir = Vector2Normalize(targetPos - startPos);
        float spread = GetRandomValue(-spreadAngle, spreadAngle) * DEG2RAD;
        dir = Vector2Rotate(dir, spread);
        velocity = Vector2Scale(dir, speed);
        // for drawing fade
        max_life = (salvo_turret_range / speed) * 2.0f;
        life = max_life;
        pierce_count = 1;
        radius = 5.0f; // slightly bigger than projectile texture to give effect of flame
    }
    void Update(float deltaTime) override
    {
        // delete projectile if too far from firing pos;
        if (Vector2DistanceSqr(position, start_pos) > GetMaxProjRange())
        {
            Destroy();
            return;
        }
        life -= deltaTime;
        position += Vector2Scale(velocity, deltaTime);
    }

    void Draw() override
    {
        float rotation = atan2f(velocity.x, velocity.y) * RAD2DEG;
        float alpha = life / max_life;
        DrawTexturePro(iceTexture, {0, 0, (float)iceTexture.width, (float)iceTexture.height}, {position.x, position.y, (float)iceTexture.width, (float)iceTexture.height}, {(float)iceTexture.width / 2, (float)iceTexture.height / 2}, rotation, Fade(WHITE, alpha));
    }
    // getter functions for parent class logic that implement something
    // based on the type/value from child classes
    ProjectileType getProjType() override { return ProjectileType::ICE_STREAM; }
    float GetMaxProjRange() override { return salvo_turret_range * salvo_turret_range; }
};

/* Shotgun bullet
* Fired by smite turret in a certain angular region
* has a high pierce count because late game turret
* Future upgrade : Maybe add bounce? 
*/

class shotgun_bullet : public Projectile
{
  public:
    shotgun_bullet(Vector2 startPos, Vector2 targetPos) : Projectile(startPos, targetPos)
    {

        velocity = velFromSpeed(startPos, targetPos, shotgun_bullet_speed); // set velocity towards target
        radius = 3.0f;
        pierce_count = 5;
    }

    void Draw() override
    {
        float rotation = atan2f(velocity.x, velocity.y) * RAD2DEG;
        DrawTexturePro(BulletTexture, {0, 0, (float)BulletTexture.width, (float)BulletTexture.height}, {position.x, position.y, (float)BulletTexture.width, (float)BulletTexture.height}, {(float)BulletTexture.width / 2, (float)BulletTexture.height / 2}, rotation, WHITE);
    }
    ProjectileType getProjType() override { return ProjectileType::SMITE; }
    float GetMaxProjRange() override { return 1 << 20; }
};

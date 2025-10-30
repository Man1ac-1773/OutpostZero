#pragma once
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <vector>

enum class ProjectileType
{
    DUO_BASIC,
    FLAME,
    SMITE,
    LASER,
    CYCLONE_BEAM,
    MELTDOWN_BEAM,
    ICE_STREAM,
};

struct Particle
{
    Vector2 pos;
    Vector2 vel;
    float life;
    float max_life;
    Color color;
    float size;
    ProjectileType proj_type;
};

class ParticleSystem
{
  public:
    std::vector<Particle> particles;

    void Update(float deltaTime)
    {
        for (auto it = particles.begin(); it != particles.end();)
        {
            it->life -= deltaTime;
            if (it->life <= 0)
            {
                it = particles.erase(it); // returns an iterator to the next element after the one destroyed
            }
            else
            {
                it->pos = Vector2Add(it->pos, Vector2Scale(it->vel, deltaTime));
                it->vel = Vector2Scale(it->vel, 0.9f);
                ++it;
            }
        }
    }

    void Draw()
    {
        BeginBlendMode(BLEND_ADDITIVE);
        for (const auto &p : particles)
        {
            float alpha = p.life / p.max_life; // how much to fade depending on fraction of life left
            DrawCircleV(p.pos, p.size, Fade(p.color, alpha));
        }
        EndBlendMode();
    }
    void Spawn(Vector2 pos, Vector2 vel, Color color, float life, float size, ProjectileType proj_type)
    {
        // placeholder for formattin
        particles.push_back({pos, vel, life, life, color, size, proj_type});
    }
    void SpawnExplosion(Vector2 center, ProjectileType proj_type)
    {

        int num_particles; // control number of particles per explosion
        float speed;
        Color color = {235, 140, 108, 255}; // magic color number, i know, i don't know what to call it, i picked it off a RGB selector on the internet

        float life = 0.0f;
        switch (proj_type)
        {
        case ProjectileType::DUO_BASIC:
        {
            num_particles = 10;
            speed = GetRandomValue(200, 700);
            life = 0.4f;
            break;
        }
        case ProjectileType::FLAME:
        {
            num_particles = 10;
            speed = GetRandomValue(100, 500);
            life = 0.7f;
            break;
        }
        case ProjectileType::LASER:
        {
            num_particles = 5;
            speed = GetRandomValue(500, 1000);
            life = 0.65f;
            break;
        }
        case ProjectileType::CYCLONE_BEAM:
        {
            num_particles = 5;
            speed = GetRandomValue(1000, 1400);
            life = 0.4f;
            break;
        }
        case ProjectileType::MELTDOWN_BEAM:
        {
            num_particles = 0;
            speed = 0;
            life = 0.0f;
            break;
        }
        case ProjectileType::ICE_STREAM:
        {
            num_particles = 5;
            speed = GetRandomValue(100, 500);
            life = 0.4f;
            color = SKYBLUE;
        }
        default:
        {
            num_particles = 10;
            speed = GetRandomValue(10, 100);
            life = 0.5f;
            break;
        }
        }

        for (int i = 0; i < num_particles; i++)
        {
            Vector2 dir = {cosf(GetRandomValue(0, 360) * DEG2RAD), sinf(GetRandomValue(0, 360) * DEG2RAD)};

            Spawn(center, Vector2Scale(dir, speed), color, life, 3.0f, proj_type);
        }
    }

    void SpawnTrail(Vector2 pos, Vector2 vel, ProjectileType proj_type)
    {
        Color c = {235, 140, 108, 255};
        Vector2 jitter = {GetRandomValue(-10, 0) / 100.0f, GetRandomValue(-10, 0) / 100.0f};
        Spawn(pos, Vector2Add(vel, jitter), c, 0.2f, 2.0f, proj_type);
    }
};

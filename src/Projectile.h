#pragma once
#include "Config.h"
#include "Entity.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include "Particles.h"

class Projectile : public Entity
{
public:
    float radius;
    Color color;
    bool active;
    Vector2 start_pos; 
    
    Projectile(Vector2 startPos, Vector2 targetPos, float speed)
    {
        start_pos = startPos; 
        position = startPos; // starting position
        radius = 1.5f; // radius of the projectile
        color = YELLOW; // color of the projectile
        active = true; // state of the projectile

        velocity = velFromSpeed(startPos, targetPos, speed); // set velocity towards target
    }
    float GetRadius() const { return radius; }
    void Update(float deltaTime) override
    {
        // delete projectile if too far from firing pos;
        if (Vector2DistanceSqr(position, start_pos) > 1 << 20) {
            Destroy();
            return;
        }
        position += Vector2Scale(velocity, deltaTime);
        particles.SpawnTrail(position, velocity);
        
   }
    void Draw() override
    {
        if (active) {
        DrawCircleV(position, radius, color);
        }
    }
};

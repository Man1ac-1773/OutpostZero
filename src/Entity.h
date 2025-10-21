#pragma once
#include "raylib.h"
class Entity
{
public:
    virtual ~Entity() {}

    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;
    bool IsActive() const { return is_active; }
    virtual void Destroy() { is_active = false; }
    Vector2 GetPosition() const { return position; }
    Vector2 GetVelocity() const { return velocity; }

// protected:
    Vector2 position;
    Vector2 velocity;
    bool is_active = true;
};

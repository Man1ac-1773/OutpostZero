#pragma once
#include "Entity.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"

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
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
  }
  void Draw() override
  {
    if (active) {
      DrawCircleV(position, radius, color);
    }
  }
};

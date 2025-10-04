#pragma once
#include <iostream> 
#include "Entity.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
using namespace std; 
class Enemy : public Entity
{
public:
  float radius;
  float speed;
  Color color;

  Enemy(Vector2 startPos, Vector2 targetPos, float speed_, float r, Color c)
  {
    position = startPos;
    radius = r;
    color = c;
    speed = speed_;

    velocity = velFromSpeed(startPos, targetPos, speed_);
  }
  float GetRadius() const { return radius; }
  void Update(float deltaTime){
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
  }
  void Draw() override
  {
    DrawCircleV(position, radius, color);
  }
  void Update(Vector2 targetPos){
    velocity = velFromSpeed(position, targetPos, speed);
  }

  

  
};
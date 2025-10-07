#pragma once
#include <iostream> 
#include "Entity.h"
#include "raylib.h"
#include <Config.h>
#include "raymath.h"
#include "utils.h"
using namespace std; 
class Enemy : public Entity
{
public:
float radius;
float speed;
Color color;
int counter = 0;
Vector2 targetPos = targets[counter]; 

Enemy(Vector2 startPos,  float speed_, float r, Color c){

    position = startPos;
    radius = r;
    color = c;
    speed = speed_;

    
}
float GetRadius() const { return radius; }
void Update(float deltaTime) override {
    targetPos = targets[counter]; 
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
}
void Draw() override {
    DrawCircleV(position, radius, color);
}
void Update(){
    if (Vector2DistanceSqr(position, targetPos) < radius*radius){
        velocity = {0,0};
        counter++; 
        return;
    }
    velocity = velFromSpeed(position, targetPos , speed);

}

  

  
};

#pragma once
#include <iostream> 
#include "Entity.h"
#include "raylib.h"
#include "Config.h"
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

Enemy(float speed_, float r, Color c){

    position = startPos;
    radius = r;
    color = c;
    speed = speed_;
}
float GetRadius() const { return radius; }
void Update(float deltaTime) override { 
    if (position.y >= GRID_ROWS*TILE_SIZE){
        Destroy();
    }
    
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
}
void Draw() override {
    DrawCircleV(position, radius, color);
}
void Update(){

    if (targetPos == targets.back()){
        targetPos.y += 40;
    }
    if (Vector2DistanceSqr(position, targetPos) < radius*radius ){
        velocity = {0,0};
        counter++;
        targetPos = targets[counter];
        return;
    }

    velocity = velFromSpeed(position, targetPos , speed);

}

};

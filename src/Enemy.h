#pragma once
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
    float hp;
    Vector2 targetPos = targets[counter]; 
    inline static int enemy_count = 0; 
Enemy(){

    position = startPos;
    enemy_count++; 
   
}
float GetRadius() const { return radius; }
void Update(float deltaTime) override { 
    if (position.y >= GRID_ROWS*TILE_SIZE){
        Destroy();
    }
    if (hp <= 0){
        Destroy();
        // DeathAnimation(); 
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
void Destroy() override {
    is_active = false; 
    enemy_count--; 
}

};

class standard_enemy : public Enemy{
    public : 
        standard_enemy(){
            radius = standard_enemy_radius;
            color = standard_enemy_color;
            speed = standard_enemy_speed;
            hp = standard_enemy_health; 
        } 

};

class fast_enemy : public Enemy{
    public: 
        fast_enemy(){
            radius = fast_enemy_radius;
            color = fast_enemy_color; 
            speed = fast_enemy_speed;
            hp = fast_enemy_health;
        }
};

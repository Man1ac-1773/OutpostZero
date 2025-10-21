#pragma once
#include "Config.h"
#include "Entity.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include "Particles.h"
#include <cmath>

enum class ProjectileState{
    SPAWNING,
    FLYING,
};


class Projectile : public Entity
{
public:
    float radius;
    Color color;
    bool active;
    Vector2 start_pos;
    Vector2 target_pos;

    
    Projectile(Vector2 startPos, Vector2 targetPos)
    {
        start_pos = startPos;
        target_pos = targetPos;
        position = startPos; // starting position
        radius = 1.5f; // radius of the projectile
        color = YELLOW; // color of the projectile
        active = true; // state of the projectile

        
    }
    virtual ProjectileType getProjType() = 0;
    virtual float doDamage() = 0;
    float GetRadius() const { return radius; }
    void Update(float deltaTime) override
    {
        // delete projectile if too far from firing pos;
        if (Vector2DistanceSqr(position, start_pos) > 1 << 20) {
            Destroy();
            return;
        }
        position += Vector2Scale(velocity, deltaTime);
        particles.SpawnTrail(position, velocity, getProjType());
        
   }
    void Draw() override
    {
        if (active) {
        DrawCircleV(position, radius, color);
        }
    }
    static void LoadTextures(){
        BulletImage = LoadImage("assets/projectiles/smallBullet.png");
        ImageResize(&BulletImage, 10,10);
        BulletTexture = LoadTextureFromImage(BulletImage); 
        UnloadImage(BulletImage); 

        // laser
        LaserImage = LoadImage("assets/projectiles/laserBullet.png");
        ImageResize(&LaserImage, 40, 2);
        LaserTexture = LoadTextureFromImage(LaserImage);

        UnloadImage(LaserImage);
    }
    static void DestroyTextures(){
        UnloadTexture(BulletTexture);
        UnloadTexture(LaserTexture);
    }
protected :
    
    // normal bullet
    inline static Image BulletImage; 
    inline static Texture2D BulletTexture;
    
    // laser bullet
    inline static Image LaserImage; 
    inline static Texture2D LaserTexture; 
};

class normal_bullet : public Projectile {
    public :
        normal_bullet(Vector2 startPos, Vector2 targetPos) : Projectile(startPos, targetPos) {
                
                velocity = velFromSpeed(startPos, targetPos, normal_bullet_speed); // set velocity towards target

        }
        

        void Draw() override {
        float rotation  = atan2f(velocity.x, velocity.y) * RAD2DEG;
        DrawTexturePro(BulletTexture, 
                        {0,0, (float)BulletTexture.width, (float)BulletTexture.height},
                        {position.x, position.y, (float)BulletTexture.width, (float)BulletTexture.height},
                        {(float)BulletTexture.width/2, (float)BulletTexture.height/2},
                        rotation,
                        WHITE); 
        
       
        }
        ProjectileType getProjType() override {return ProjectileType::DUO_BASIC;}
        float doDamage()override{return normal_bullet_damage;} 
   
     

};

class laser_bullet: public Projectile {
    public :
        laser_bullet(Vector2 startPos, Vector2 targetPos) : Projectile(startPos, targetPos) {
            velocity = velFromSpeed(startPos,targetPos, laser_bullet_speed);
            spawnTimer = m_spawnTimer; 
            state = ProjectileState::SPAWNING; 
        }
        void Update(float deltaTime) override
        {
            // delete projectile if too far from firing pos;
                if (Vector2DistanceSqr(position, start_pos) > 1 << 20) {
                    Destroy();
                    return;
            
                }
               
            switch (state){
                case ProjectileState::SPAWNING: 
                { 
                    spawnTimer -= deltaTime; 
                    if (spawnTimer <= 0){state = ProjectileState::FLYING;}
                    break;
                }
                case ProjectileState::FLYING:
                {
                    position += Vector2Scale(velocity, deltaTime);
                    break;
                }
            }
        }

        void Draw() override {
            float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG;
            float toRender;  
            if (state == ProjectileState::SPAWNING) {
                toRender = (m_spawnTimer- spawnTimer) / m_spawnTimer;
            }
            else { 
                toRender = 1.0f;    
            }
            Rectangle sourceRec = { 0, 0, (float)LaserTexture.width*toRender, (float)LaserTexture.height };
            Rectangle destRec = { position.x, position.y, (float)LaserTexture.width*toRender, (float)LaserTexture.height };
    
            Vector2 origin = { 0, (float)LaserTexture.height / 2 };

            BeginBlendMode(BLEND_ADDITIVE);
                DrawTexturePro(LaserTexture, sourceRec, destRec, origin, rotation, WHITE);
            EndBlendMode();
        }
        
        ProjectileType getProjType() override {return ProjectileType::LASER_BASIC;}
        float doDamage()override{return laser_bullet_damage;} 

    private:
        inline static float m_spawnTimer = 0.04f; 
        float spawnTimer; 
        ProjectileState state;

        
   
};




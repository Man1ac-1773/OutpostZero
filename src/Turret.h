#pragma once
#include "Enemy.h"
#include "Entity.h"
#include "Projectile.h"
#include "raylib.h"
#include "raymath.h"
#include "Map.h"
#include <Config.h>
#include "utils.h"
#include <memory> // for std::unique_ptr
#include <vector>

class Turret : public Entity
{
public:
  // base properties
  float radius;
  float range;
  Color color;
  // shooting properties
  float fireRate;
  float fireTimer;

  // gun properties
  Rectangle gunRec; 
  float gunRotation;
  float rotationSpeed; 
  Color gunColor;
  Tile tileOfTurret; 

Turret(Vector2 pos, Tile& tile)
  {
    position = pos; // position of the turret
    gunRotation = 0.0f; // initial rotation of the gun
    tileOfTurret = tile; 
  }

void Update(float deltaTime) override
  {
    // primitive update function 
    if (fireTimer > 0) {
      fireTimer -= deltaTime;
    }
  }
void Update(float deltaTime,
               const std::vector<Enemy*>& targets,
               std::vector<std::unique_ptr<Entity>>& newProjectiles)
  {
      Enemy* target = nullptr;
      float closestDist = 999999.0f; 

      for (auto& e_ptr : targets) {
        float dist = Vector2Distance(position, e_ptr->GetPosition());
        if (dist < range && dist < closestDist) {
          closestDist = dist;
          target = e_ptr;
        }
      }
      if (target != nullptr) {
        float projectileSpeed = 400.0f;
        Vector2 aimPoint = CalculateInterceptPoint(
          target->GetPosition(), target->GetVelocity(), projectileSpeed);
          
        float targetAngle =
          atan2f(aimPoint.y - position.y, aimPoint.x - position.x) * RAD2DEG;
          
        float rotationSpeed = 5.0f;
        gunRotation = MoveAngle(gunRotation, targetAngle, rotationSpeed);
        float angleDifference = normaliseAngle(targetAngle - gunRotation);
        if (fireTimer <= 0 && angleDifference < 8.0f) {
          newProjectiles.push_back(
          std::make_unique<Projectile>(position, aimPoint, projectileSpeed));
          fireTimer = fireRate; // reset the cooldown
        }
      }
  }
void Draw() override
{
    // Draw the range first (semi-transparent)
    // DrawCircleV(position, range, Fade(color, 0.2f));
    // Draw the base
    DrawCircleV(position, radius, color);
    // Draw the gun
    DrawRectanglePro(gunRec,
                     Vector2{ 0, gunRec.height / 2 },
                     gunRotation,
                     gunColor);
}

void drawRangeOnHover(Vector2 pos){
    if (CheckCollisionPointRec(pos, tileOfTurret.rect)){
        DrawCircleLines(position.x, position.y, range, YELLOW);
    }
    
}

static void loadTextures(){
    turretBaseIMG = LoadImage("assets/turrets/base-1.png");
    turretBaseTexture = LoadTextureFromImage(turretBaseIMG);

    basicTurretGunIMG = LoadImage("assets/turrets/duo.png");
    basicTurretGunTexture = LoadTextureFromImage(basicTurretGunIMG);
    UnloadImage(turretBaseIMG); 
    UnloadImage(basicTurretGunIMG);
}
static void destroyTextures(){
    UnloadTexture(turretBaseTexture);
    UnloadTexture(basicTurretGunTexture);
}

protected:
    // the base of each turret. Common
    inline static Image turretBaseIMG; 
    inline static Texture2D turretBaseTexture; 

    // basic_turret stuff
    inline static Image basicTurretGunIMG; 
    inline static Texture2D basicTurretGunTexture; 


    // Aim functionality, can be made worse to be poor turret 
    Vector2 CalculateInterceptPoint(Vector2 enemyPos,
                                      Vector2 enemyVel,
                                      float projectileSpeed)
    {
        Vector2 toEnemy = enemyPos - position;
        float a = Vector2LengthSqr(enemyVel) - projectileSpeed * projectileSpeed;
        float b = 2 * Vector2DotProduct(toEnemy, enemyVel);
        float c = Vector2LengthSqr(toEnemy);
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0 || fabs(a) < 0.0001f) {
            return enemyPos; // No solution, aim directly at the enemy
        } 
        else {
          float sqrtDisc = sqrt(discriminant);
          float t1 = (-b - sqrtDisc) / (2 * a);
          float t2 = (-b + sqrtDisc) / (2 * a);
          float t = (t1 > 0) ? t1 : ((t2 > 0) ? t2 : -1);

            if (t > 0) {
                return Vector2Add(enemyPos, Vector2Scale(enemyVel, t));
            } 
            else {
            return enemyPos; // Both times are negative, aim directly at the enemy
            }
        }
    }

};


class basic_turret : public Turret {
    public:
         
        basic_turret(Vector2 pos, Tile& tile) : Turret(pos, tile){
            range = 3*TILE_SIZE;
            fireRate = 0.25f;  // shoots 1/x per second
            fireTimer = 0.0f; // initial timer, ready to fire
            rotationSpeed = 0.5f;
        }
        
        
        void Draw() override {
            Vector2 baseOrigin = { (float)turretBaseTexture.width / 2.0f, (float)turretBaseTexture.height / 2.0f };
            
            DrawTexturePro(turretBaseTexture,
                   { 0, 0, (float)turretBaseTexture.width, (float)turretBaseTexture.height }, 
                   { position.x, position.y, (float)turretBaseTexture.width, (float)turretBaseTexture.height }, 
                   baseOrigin,
                   0.0f, 
                   WHITE);

            Vector2 gunOrigin = { (float)basicTurretGunTexture.width / 2.0f, (float)basicTurretGunTexture.height * 0.75f - 2.0f };
            DrawTexturePro(basicTurretGunTexture,
                   { 0, 0, (float)basicTurretGunTexture.width, (float)basicTurretGunTexture.height }, 
                   { position.x-2.0f, position.y, (float)basicTurretGunTexture.width, (float)basicTurretGunTexture.height }, 
                   gunOrigin,
                   gunRotation + 90.0f, 
                   WHITE);  

        }

};

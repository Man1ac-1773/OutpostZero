#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cmath>

struct Particle{
    Vector2 pos; 
    Vector2 vel; 
    float life; 
    float max_life; 
    Color color; 
    float size; 
};

class ParticleSystem {
    public:
        std::vector<Particle> particles;
        
        void Spawn(Vector2 pos, Vector2 vel, Color color, float life, float size) {
            particles.push_back({pos, vel, life, life, color, size});
        }        
       
        void Update(float deltaTime){
            for (auto it = particles.begin(); it != particles.end();){
                it->life -= deltaTime; 
                if (it->life <= 0){
                    it = particles.erase(it); // returns an iterator to the next element after the one destroyed
                }
                else{
                    it->pos = Vector2Add(it->pos, Vector2Scale(it->vel, deltaTime)); 
                    it->vel = Vector2Scale(it->vel, 0.9f);
                    ++it;
                }
                
            }
        }

        void Draw() const {
            BeginBlendMode(BLEND_ADDITIVE);
            for (const auto& p: particles){
                float alpha = p.life/p.max_life; // how much to fade depending on fraction of life left 
                DrawCircleV(p.pos, p.size , Fade(p.color, alpha));
            }
            EndBlendMode(); 
        }
        
        void SpawnExplosion(Vector2 center) {
            for (int i = 0; i < 10; i++) {
                Vector2 dir = {cosf(GetRandomValue(0, 360) * DEG2RAD),
                               sinf(GetRandomValue(0, 360) * DEG2RAD)};
                float spd = GetRandomValue(50,200);

                Spawn(center, Vector2Scale(dir, spd), RED, 0.2f , 3.0f);
            }
        }

        void SpawnTrail(Vector2 pos, Vector2 vel) {
            Color c = {255, 240, 80, 255};
            Vector2 jitter = { GetRandomValue(-10, 10) / 100.0f, GetRandomValue(-10, 10) / 100.0f };
            Spawn(pos, Vector2Add(vel, jitter), c, 0.2f, 2.0f);
        }
};


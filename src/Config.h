#pragma once
#include <Particles.h>
#include <raylib.h>
#include <vector>
// initial screen dimensions
extern int screenWidth;
extern int screenHeight;

// intro
extern const int introBoxWidth;
extern const int introBoxHeight;
extern const int introBoxSpacing;
extern const int introMaxWidth;
extern const int introMaxHeight;
extern const int introZoomSpeed;
// game

// colors
extern const Color pathColor;
extern const Color buildableColor;
extern const Color gridLineColor;
extern const Color mouseHoverColor;

// path followed by enemy
extern std::vector<Vector2> targets;
extern Vector2 startPos;
// player
extern int player_health;

// map constants
extern const float TILE_SIZE; // Each tile is NxN pixels
extern const int GRID_COLS;
extern const int GRID_ROWS;

// constants for enemies
// for standard_enemy
extern const float standard_enemy_radius;
extern const Color standard_enemy_color;
extern const float standard_enemy_speed;
extern const float standard_enemy_health;

// for fast_enemy
extern const float fast_enemy_radius;
extern const float fast_enemy_speed;
extern const Color fast_enemy_color;
extern const float fast_enemy_health;

// particles
extern ParticleSystem particles;
// TURRETS
// PROJECTILE TURRETS
// duo turret
extern const float duo_turret_fire_rate;
extern const float duo_turret_range;

// LASER TURRETS
// Scatter turret
extern const float scatter_turret_fire_rate;
extern const float scatter_turret_range;

// Cyclone turret
extern const float cyclone_turret_fire_rate;
extern const float cyclone_turret_range;

// SLOWING TURRET
// slowing turret
extern const float wave_turret_active_time;
extern const float wave_turret_cooldown_time;
extern const float wave_turret_range;
// projectiles
// normal bullet
extern const float normal_bullet_speed;
extern const float normal_bullet_damage;

// laser bullets;
extern const float laser_bullet_speed;
extern const float laser_bullet_damage;

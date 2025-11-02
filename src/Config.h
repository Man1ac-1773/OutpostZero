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
// for flare_enemy
extern const float flare_enemy_radius;
extern const float flare_enemy_speed;
extern const float flare_enemy_health;

// for mono_enemy
extern const float mono_enemy_radius;
extern const float mono_enemy_speed;
extern const float mono_enemy_health;

// for crawler enemy
extern const float crawler_enemy_radius;
extern const float crawler_enemy_speed;
extern const float crawler_enemy_health;

// for poly enemy
extern const float poly_enemy_radius;
extern const float poly_enemy_health;
extern const float poly_enemy_speed;
// particles
extern ParticleSystem particles;
// TURRETS
// PROJECTILE TURRETS
// duo turret
extern const float duo_turret_fire_rate;
extern const float duo_turret_range;
// ripple turret => flamethrower
extern const float ripple_turret_fire_rate;
extern const float ripple_turret_range;
// smite turret => shotgun
extern const float smite_turret_fire_rate;
extern const float smite_turret_range;
// LASER TURRETS
// lancer turret
extern const float lancer_turret_fire_rate;
extern const float lancer_turret_range;

// Cyclone turret
extern const float cyclone_turret_range;
extern const float cyclone_turret_beam_timer;
extern const float cyclone_turret_cooldown_timer;

// Meltdown turret
extern const float meltdown_turret_range;
extern const float meltdown_turret_beam_timer;
extern const float meltdown_turret_cooldown_timer;
extern const float meltdown_turret_dps;

// SLOWING TURRET
// wave turret
extern const float wave_turret_active_time;
extern const float wave_turret_cooldown_time;
extern const float wave_turret_range;
// salvo
extern const float salvo_turret_range;
extern const float salvo_turret_fire_rate;
// projectiles
// normal bullet
extern const float normal_bullet_speed;
extern const float normal_bullet_damage;
// flame
extern const float flame_bullet_damage;
extern const float flame_bullet_speed;
// shotgun
extern const float shotgun_bullet_damage;
extern const float shotgun_bullet_speed;
// laser bullets;
extern const float lancer_bullet_speed;
extern const float lancer_bullet_damage;

// cyclone
extern const float cyclone_beam_damage;

// salvo
extern const float ice_stream_speed;
extern const float ice_stream_damage;

// helper
extern const float max_heal_cooldown;

#include "Config.h"
#include "Particles.h"
#include <raylib.h>
// initial screen dimensions
int screenWidth = 1000;
int screenHeight = 600;

// intro
const int introBoxWidth = 250;
const int introBoxHeight = 75;
const int introBoxSpacing = 25;
const int introMaxWidth = 275;
const int introMaxHeight = 100;
const int introZoomSpeed = 4;

// colors
const Color pathColor = Color{100, 100, 100, 255};
const Color buildableColor = Color{80, 80, 80, 255};
const Color gridLineColor = Color{60, 60, 60, 100}; // colour of border of each tile
const Color mouseHoverColor = Color{100, 255, 100, 50};

// players
int player_health = 5;

// targets
// has been declared here because of some (possible?) use in other files, otherwise is local to the enemy class and is used there only.
std::vector<Vector2> targets;
Vector2 startPos;

/* TILE SYSTEM CONSTANTS
 * the GRID_COLS variable is fine tuned to the layout of the map
 * Always change it in multiples of 3.
 * Keep adding 3 basically, other than that, the map will be faulty
 */
const int GRID_ROWS = 14;
const int GRID_COLS = 21;
const float TILE_SIZE = 40.0f;

// ---- CONSTANTS FOR ENEMIES ----
// for flare_enemy
const float flare_enemy_radius = 12.0f;
const float flare_enemy_speed = 100.0f;
const float flare_enemy_health = 10.0f;

// for mono_enemy
const float mono_enemy_radius = 6.0f;
const float mono_enemy_speed = 2.0f * flare_enemy_speed;
const float mono_enemy_health = 5.0f;

// for crawler enem
const float crawler_enemy_radius = 6.0f;
const float crawler_enemy_speed = 1.5f * flare_enemy_speed;
const float crawler_enemy_health = mono_enemy_health;

// for poly enemy
const float poly_enemy_radius = 8.0f;
const float poly_enemy_speed = flare_enemy_speed;
const float poly_enemy_health = mono_enemy_health;

// for locus_enemy
const float locus_enemy_health = 20.0f;
const float locus_enemy_speed = 75.0f;
const float locus_enemy_radius = 16.0f;
// ---- ----

// particle system
ParticleSystem particles;

/* ---- CONSTANTS FOR TURRETS AND PROJECTILES ----
 * DISCLAIMER FOR CONSTANT RELATIONS
 * duo_turret -> normal_bullet
 * lancer_turret -> laser_bullet
 * more types will come later
 */
// duo turret
const float duo_turret_fire_rate = 4.0f; // bullets/second
const float duo_turret_range = 3.0f * TILE_SIZE;

// ripple turret
const float ripple_turret_fire_rate = 1000.0f;
const float ripple_turret_range = 3.0f * TILE_SIZE;

// smite turret
const float smite_turret_fire_rate = 1.0f;
const float smite_turret_range = 5.0f * TILE_SIZE;
// lancer turret
const float lancer_turret_fire_rate = 1.0f;
const float lancer_turret_range = 6.0f * TILE_SIZE;

// cyclone turret
const float cyclone_turret_range = 9.0f * TILE_SIZE;
const float cyclone_turret_beam_timer = 0.080f;
const float cyclone_turret_cooldown_timer = 0.6f;
// meltdown turret
const float meltdown_turret_range = 5.0f * TILE_SIZE;
const float meltdown_turret_cooldown_timer = 2.0f;
const float meltdown_turret_beam_timer = 5.0f;
const float meltdown_turret_dps = 20.0f;

// wave turret
const float wave_turret_range = 3.0f * TILE_SIZE;
const float wave_turret_active_time = 15.0f;
const float wave_turret_cooldown_time = 5.0f;
// salvo turret
const float salvo_turret_fire_rate = ripple_turret_fire_rate;
const float salvo_turret_range = wave_turret_range * 1.5f;
/* Projectiles */
// normal
const float normal_bullet_speed = 400.0f;
const float normal_bullet_damage = 5.0f;
// flame
const float flame_bullet_damage = 0.5f;
const float flame_bullet_speed = 200.0f;
// shotgun
const float shotgun_bullet_damage = 5.0f;
const float shotgun_bullet_speed = 400.0f;
// laser
const float lancer_bullet_speed = 1000.0f;
const float lancer_bullet_damage = 10.0f;

// cyclone
const float cyclone_beam_damage = 10.0f; // theoretical base damage
                                         // multiplier will act on this depending on range

// salvo
const float ice_stream_damage = 0.1f;
const float ice_stream_speed = flame_bullet_speed * 1.5f;

// helpers
const float max_heal_cooldown = 2.0f;

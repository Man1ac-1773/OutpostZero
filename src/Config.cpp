#include "Config.h"
#include "Particles.h"
#include <raylib.h>
// initial screen dimensions
int screenWidth = 1200;
int screenHeight = 650;

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
int player_health = 10; // max enemies that can cross u
int playerMoney = 250; // money u start with it. Allows for more diverse openings.
float enemy_health_multiplier = 1.0f; // Global multiplier for enemy health

// turret costs
const int duo_turret_cost = 100;
const int ripple_turret_cost = 400;    // T2 Upgrade - Made slightly more affordable
const int smite_turret_cost = 800;     // T3 Upgrade
const int lancer_turret_cost = 200;
const int cyclone_turret_cost = 450;   // T2 Upgrade - Made slightly more affordable
const int meltdown_turret_cost = 1000; // T3 Upgrade
const int wave_turret_cost = 250;      // Lowered cost to encourage building it for utility before wave 7.
const int salvo_turret_cost = 500;     // T2 Upgrade

// enemy kill rewards
const int flare_enemy_reward = 10;     // Basic enemy, lower reward.
const int mono_enemy_reward = 15;      // Fast but weak, slightly higher reward.
const int crawler_enemy_reward = 30;   // Increased reward for a tricky enemy.
const int poly_enemy_reward = 20;      // Healer, moderate reward.
const int locus_enemy_reward = 40;     // Tank, good reward.
const int antumbra_enemy_reward = 1000; // The reward for killing the boss.

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
const float flare_enemy_speed = 90.0f;  // Slightly slower, but more durable.
const float flare_enemy_health = 12.0f; // Requires 3 duo shots, or 2 lancer shots. Incentivizes Lancer.

// for mono_enemy
const float mono_enemy_radius = 6.0f;
const float mono_enemy_speed = 2.0f * flare_enemy_speed;
const float mono_enemy_health = 9.0f;   // Requires 2 duo shots, but only 1 lancer shot. This makes Lancer a strong counter.
// for crawler enemy
const float crawler_enemy_radius = 6.0f;
const float crawler_enemy_speed = 1.5f * flare_enemy_speed;
const float crawler_enemy_health = 15.0f; // Tougher to kill to make them more of a threat.

// for poly enemy
const float poly_enemy_radius = 8.0f;
const float poly_enemy_speed = locus_enemy_speed; // Match speed with Locus to act as a proper support unit.
const float poly_enemy_health = 20.0f;  // Healers should be a bit more durable.
const float poly_enemy_heal_amount = 2.5f; // Amount healed per tick.

// for locus_enemy
const float locus_enemy_health = 150.0f; // A true tank. Duo spam will struggle significantly. This demands an upgrade.
const float locus_enemy_speed = 75.0f;
const float locus_enemy_radius = 16.0f;

// for antumbra enemy (boss)
const float antumbra_enemy_health = 2000.0f; // Boss health, buffed for late-game scaling
const float antumbra_enemy_speed = 50.0f;    // Slower than Locus
const float antumbra_enemy_radius = 32.0f;   // Larger radius
// ---- ----

// particle system
ParticleSystem particles;

/* ---- CONSTANTS FOR TURRETS AND PROJECTILES ----
 * DISCLAIMER FOR CONSTANT RELATIONS
 * duo_turret -> normal_bullet
 * lancer_turret -> laser_bullet
 * ripple_turret -> flame_bullet
 * smite_turret -> shotgun_bullet
 * cyclone_turret -> cyclone_beam
 * salvo_turret -> ice_stream
 * 
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
const float lancer_turret_fire_rate = 1.5f; // Slight buff to make it a more attractive choice.
const float lancer_turret_range = 6.0f * TILE_SIZE;

// cyclone turret
const float cyclone_turret_range = 9.0f * TILE_SIZE;
const float cyclone_turret_beam_timer = 0.06f;
const float cyclone_turret_cooldown_timer = 0.8f; // Slower fire rate
const int cyclone_turret_max_pierce_count = 5;    // Increased pierce
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
const float flame_bullet_damage = 1.0f;
const float flame_bullet_speed = 200.0f;
const float flame_bullet_spread = 15.0f; // degrees
// shotgun
const float shotgun_bullet_damage = 5.0f;
const float shotgun_bullet_speed = 400.0f;
// laser
const float lancer_bullet_speed = 1000.0f;
const float lancer_bullet_damage = 10.0f;

// cyclone
const float cyclone_beam_damage = 40.0f; // Significant damage buff
                                         // multiplier will act on this depending on range

// salvo
const float ice_stream_damage = 0.1f; // damage per projectile
const float ice_stream_speed = flame_bullet_speed * 1.5f; // faster than flame bullets
const float ice_stream_spread = 5.0f; // degrees

// helpers
const float max_heal_cooldown = 1.0f; // Reduced from 2.0 to double the healing frequency.

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

// targets
std::vector<Vector2> targets;
Vector2 startPos;

/* TILE SYSTEM CONSTANTS
 * the GRID_COLS variable is fine tuned to the layout of the map
 * Always change it in multiples of 3.
 * Keep adding 3 basically, other than that, the map will be faulty
 */
const int GRID_ROWS = 14;
const int GRID_COLS = 21;
const int TILE_SIZE = 40;

// constants for enemies
// for standard_enemy
const float standard_enemy_radius = 12.0f;
const float standard_enemy_speed = 100.0f;
const float standard_enemy_health = 10.0f;

// for fast_enemy
const float fast_enemy_radius = 6.0f;
const float fast_enemy_speed = 2 * standard_enemy_speed;
const float fast_enemy_health = 5.0f;

// particle system
ParticleSystem particles;
// turrets
// duo turret
const float duo_turret_fire_rate = 4.0f; // bullets/second
                                         //
// cyclone turret
const float cyclone_turret_fire_rate = 1.0f; // bullets/second
                                             // projectiles
// normal
const float normal_bullet_speed = 400.0f;
const float normal_bullet_damage = 5.0f;
// laser
const float laser_bullet_speed = 1000.0f;
const float laser_bullet_damage = 10.0f;

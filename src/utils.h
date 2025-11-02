#pragma once
#include "raylib.h"
#include "raymath.h"

/* Utils.h file to help with math functions and other minor stuff
 *
 * "Why no .cpp" => I'm lazy. Why maintain so many files when you can just maintain one.
 *
 * "Why are they all inline". Because this is the header file. During compilation this file is copy pasted into every .cpp file that includes it.
 *
 * This means that when the .cpp files are compiled together, and merged with other files in a single compilation unit, the linker sees multiple definitions of this SAME function, and immediately slaps u with a "ODR violation" (I hate ODR).
 *
 * To prevent that, we use the keyword inline.
 *
 * IT basically tells the linker that "you will see multiple definitions of this. Use any one. I guarantee they are all the same".
 */

// return value of velocity as a Vector2
inline Vector2 velFromSpeed(const Vector2 &startPos, const Vector2 &targetPos, float speed) { return Vector2Scale(Vector2Normalize(targetPos - startPos), speed); }
// it is a simple one-liner, I don't even know why i created a seperate function for it
// "Felt cute, might delete later" lol

// make the gun move a certain angle while continously attempting to
// -360 < theta < 360
inline float MoveAngle(float current, float target, float maxDelta)
{
    float diff = target - current;
    while (diff > 180.0f)
        diff -= 360.0f;
    while (diff < -180.0f)
        diff += 360.0f;

    if (abs(diff) < maxDelta)
    {
        return target;
    }
    if (diff > 0)
    {
        return current + maxDelta;
    }
    else
    {
        return current - maxDelta;
    }
}
// -360 < theta < 360
inline float normaliseAngle(float angle)
{
    while (angle < 0)
        angle += 360.0f;
    while (angle >= 360.0f)
        angle -= 360.0f;
    return angle;
}

// helper function to draw health bar on top of any entity,
// seperated and kept here to make code scaleable and this functionality reusable
inline void DrawHealthBar(float curr_health, float max_health, Vector2 entity_pos)
{
    float bar_thickness = 5.0f;
    float bar_width = 20.0f;
    Rectangle healthBar = {entity_pos.x - bar_width / 2, entity_pos.y - bar_thickness * 4, bar_width, bar_thickness};
    Rectangle currHealth = {entity_pos.x - bar_width / 2, entity_pos.y - bar_thickness * 4, bar_width * curr_health / max_health, bar_thickness};

    DrawRectangleRec(healthBar, GRAY);
    DrawRectangleRec(currHealth, RED);
}

/* Calculate damage falloff based off of how far enemy is
 * distance => square distance of enemy to turret
 * max_range => of turret
 * Goal is to return 1 at max_range, and scale it larger as it goes closer
 * but clamp it at some value
 */
inline float GetDamageFalloff(float distSqr, float range, int enemiesHit = 0)
{

    float distRatio;
    // range 0 is passed when we don't want range based falloff
    if (range <= 0.0f)
    {
        distRatio = 0.0f;
    }
    else
        distRatio = distSqr / (range * range);

    float distanceFalloff = 1.0f - distRatio;

    float pen = 1.0f / (1.0f + 0.5f * enemiesHit);

    float damage = distanceFalloff * pen;

    Clamp(damage, 0, 1); // limit values, just in case

    return damage;
}

#pragma once
#include "raylib.h"
#include "raymath.h"

// return value of velocity as a Vector2
inline Vector2 velFromSpeed(const Vector2 &startPos, const Vector2 &targetPos, float speed) { return Vector2Scale(Vector2Normalize(targetPos - startPos), speed); }

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

inline float Clamp01(float value) { return Clamp(value, 0.0f, 1.0f); } // clamp a value between 0 and 1, using raymath helper

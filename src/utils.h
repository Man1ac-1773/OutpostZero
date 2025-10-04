#pragma once
#include "raylib.h"
#include "raymath.h"

inline Vector2
velFromSpeed(const Vector2& startPos, const Vector2& targetPos, float speed)
{
    Vector2 direction = Vector2Normalize({ targetPos.x-startPos.x, targetPos.y-startPos.y });
   
    return { direction.x * speed, direction.y * speed };
}

inline float MoveAngle(float current, float target, float maxDelta) {
    float diff = target - current;
    while (diff > 180.0f) diff -= 360.0f;
    while (diff < -180.0f) diff += 360.0f;

    
    if (abs(diff) < maxDelta) {
        return target; 
    }
    if (diff > 0) {
        return current + maxDelta;
    } else {
        return current - maxDelta;
    }
}
inline float normaliseAngle(float angle) {
    while (angle < 0) angle += 360.0f;
    while (angle >= 360.0f) angle -= 360.0f;
    return angle;
}
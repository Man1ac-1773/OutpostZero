#pragma once

// types of different entity declaration
enum class ProjectileType
{
    DUO_BASIC,
    FLAME,
    SMITE,
    LASER,
    CYCLONE_BEAM,
    MELTDOWN_BEAM,
    ICE_STREAM,
};
enum class TurretType
{
    // projectile turrets
    DUO,
    RIPPLE,
    SMITE,
    // laser turrets
    LANCER,
    CYCLONE,
    MELTDOWN,
    // effects turrets
    WAVE,
    SALVO,
};
enum class StatusEffects
{
    NONE,
    SLOWED,
};
enum class EnemyType
{
    FLARE,
    MONO,
    CRAWLER,
    POLY,
    LOCUS,
    ANTUMBRA,
};
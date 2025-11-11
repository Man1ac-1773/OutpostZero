#pragma once

#include "Config.h"
#include "Enemy.h"
#include "Entity.h"
#include <memory>
#include <vector>
using namespace std;
struct SpawnCommand
{
    EnemyType type;
    float delayUntilNext; // time to next spawn
};

// --- WaveManager Class ---
// Handles spawning of waves and storing their data
// waits for user to press the button
class WaveManager
{
  public:
    // getters, in case need to call from outside
    int GetWaveNumber() { return currentWaveIndex + 1; } // Returns the current wave number (1-indexed)
    int GetStageNumber() { return currentStage; } // Returns the current difficulty stage
    bool IsWaveActive() { return state == State::SPAWNING || state == State::WAVE_IN_PROGRESS; }       
    int GetWavesUntilBoss() { return BOSS_WAVE_INTERVAL - (currentWaveIndex % BOSS_WAVE_INTERVAL); }
    int GetTotalWaves() { return (int)(allWaveScripts.size()); }
    bool IsFinished() { return state == State::FINISHED; }

    const int BOSS_WAVE_INTERVAL = 10; // A boss appears every 10 waves

    bool CanStartNextWave() { return state == State::WAITING_FOR_PLAYER; }

    WaveManager()
    {
        // 'allWaveScripts' is a vector of wave scripts
        // a wave script is a vector of SpawnCommands

        // wave 1: 10 flare enemies, 1 second apart
        allWaveScripts.push_back({{EnemyType::FLARE, 1.0f}, {EnemyType::FLARE, 1.0f}, {EnemyType::FLARE, 1.0f}, {EnemyType::FLARE, 1.0f}, {EnemyType::FLARE, 1.0f}, {EnemyType::FLARE, 1.0f}, {EnemyType::FLARE, 1.0f}, {EnemyType::FLARE, 1.0f}, {EnemyType::FLARE, 1.0f}, {EnemyType::FLARE, 1.0f}});

        // wave 2: Introduce fast enemies
        allWaveScripts.push_back({{EnemyType::FLARE, 0.8f}, {EnemyType::FLARE, 0.8f}, {EnemyType::FLARE, 0.8f}, {EnemyType::FLARE, 0.8f}, {EnemyType::FLARE, 0.8f}, {EnemyType::MONO, 0.5f}, {EnemyType::MONO, 0.5f}, {EnemyType::MONO, 0.5f}, {EnemyType::MONO, 0.5f}, {EnemyType::MONO, 0.5f}});

        // wave 3 => made it tighter: alternating flares and monos
        allWaveScripts.push_back({{EnemyType::FLARE, 0.4f}, {EnemyType::MONO, 0.4f}, {EnemyType::FLARE, 0.4f}, {EnemyType::MONO, 0.4f}, {EnemyType::FLARE, 0.4f},
                                  {EnemyType::MONO, 0.4f}, {EnemyType::FLARE, 0.4f}, {EnemyType::MONO, 0.4f}, {EnemyType::FLARE, 0.4f}, {EnemyType::MONO, 0.4f}});

        // wave 4: A true test of damage output. The Locus is now embedded in a much denser stream of Flares.
        allWaveScripts.push_back({
            {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f},
            {EnemyType::LOCUS, 0.3f}, // The Locus now appears right in the middle of the wave.
            {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}, {EnemyType::FLARE, 0.3f}
        });
        // wave 5: A much denser wave of fast enemies to overwhelm basic defenses.
        allWaveScripts.push_back({{EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f},
                                  {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}, {EnemyType::MONO, 0.15f}});

        // wave 6: Healers now spawn right behind their tanks, making them a priority target.
        allWaveScripts.push_back({{EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 2.0f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.2f}, {EnemyType::POLY, 2.0f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.2f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.2f}});

        // wave 7: A mix of invisible crawlers and fast monos to split the player's focus.
        allWaveScripts.push_back({{EnemyType::CRAWLER, 0.8f}, {EnemyType::MONO, 0.4f}, {EnemyType::CRAWLER, 0.8f}, {EnemyType::MONO, 0.4f}, {EnemyType::CRAWLER, 0.8f}, {EnemyType::MONO, 0.4f}, {EnemyType::CRAWLER, 0.8f}, {EnemyType::MONO, 0.4f}, {EnemyType::CRAWLER, 0.8f}, {EnemyType::MONO, 0.4f},
                                  {EnemyType::CRAWLER, 0.8f}, {EnemyType::MONO, 0.4f}});

        // wave 8: A pre-boss challenge focusing on overwhelming durable enemies.
        allWaveScripts.push_back({
            {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f},
            {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}
        });

        // wave 9: A pre-boss challenge focusing on overwhelming numbers and speed.
        allWaveScripts.push_back({
            {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f},
            {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f},
            {EnemyType::CRAWLER, 0.2f}, {EnemyType::CRAWLER, 0.2f}, {EnemyType::CRAWLER, 0.2f}, {EnemyType::CRAWLER, 0.2f}, {EnemyType::CRAWLER, 0.2f}, {EnemyType::CRAWLER, 0.2f}, {EnemyType::CRAWLER, 0.2f}, {EnemyType::CRAWLER, 0.2f}, {EnemyType::CRAWLER, 0.2f}, {EnemyType::CRAWLER, 0.2f}
        });

        // wave 10: BOSS WAVE - The Antumbra appears with supporting minions.
        allWaveScripts.push_back({
            {EnemyType::FLARE, 0.5f}, {EnemyType::FLARE, 0.5f}, {EnemyType::FLARE, 0.5f}, {EnemyType::FLARE, 0.5f}, {EnemyType::FLARE, 5.0f}, // First minion wave
            {EnemyType::ANTUMBRA, 0.1f}, // The Boss appears!
            {EnemyType::MONO, 1.0f}, {EnemyType::MONO, 1.0f}, {EnemyType::MONO, 1.0f}, {EnemyType::MONO, 1.0f}, {EnemyType::MONO, 5.0f}, // Second minion wave to distract
            {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, // A tank-healer pair to complicate things
            {EnemyType::MONO, 1.0f}, {EnemyType::MONO, 1.0f}, {EnemyType::MONO, 1.0f}, {EnemyType::MONO, 1.0f} // Final minion wave
        });

        // --- STAGE 2 WAVES ---
        // wave 11: A dense swarm of tougher basic enemies.
        allWaveScripts.push_back({
            {EnemyType::FLARE, 0.2f}, {EnemyType::FLARE, 0.2f}, {EnemyType::MONO, 0.2f}, {EnemyType::FLARE, 0.2f}, {EnemyType::MONO, 0.2f}, {EnemyType::FLARE, 0.2f}, {EnemyType::MONO, 0.2f}, {EnemyType::FLARE, 0.2f}, {EnemyType::MONO, 0.2f}, {EnemyType::FLARE, 0.2f},
            {EnemyType::FLARE, 0.2f}, {EnemyType::MONO, 0.2f}, {EnemyType::FLARE, 0.2f}, {EnemyType::MONO, 0.2f}, {EnemyType::FLARE, 0.2f}, {EnemyType::MONO, 0.2f}, {EnemyType::FLARE, 0.2f}, {EnemyType::MONO, 0.2f}, {EnemyType::FLARE, 0.2f}, {EnemyType::MONO, 0.2f}
        });

        // wave 12: A serious tank check with multiple Locus/Poly pairs.
        allWaveScripts.push_back({
            {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 3.0f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 3.0f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 3.0f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 3.0f}
        });

        // wave 13: A massive, continuous stream of fast enemies.
        allWaveScripts.push_back(vector<SpawnCommand>(40, {EnemyType::MONO, 0.1f}));

        // wave 14: A true invisibility test.
        allWaveScripts.push_back({
            {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f},
            {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::CRAWLER, 0.5f}
        });

        // wave 15: A chaotic mix of everything learned so far.
        allWaveScripts.push_back({
            {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, {EnemyType::MONO, 0.1f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::MONO, 0.1f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, {EnemyType::CRAWLER, 0.5f}, {EnemyType::MONO, 0.1f}, {EnemyType::MONO, 0.1f}
        });

        // wave 16, 17, 18, 19: Placeholder for future complex waves. For now, we repeat wave 15 to ramp up.
        allWaveScripts.push_back(allWaveScripts.back());
        allWaveScripts.push_back(allWaveScripts.back());
        allWaveScripts.push_back(allWaveScripts.back());
        allWaveScripts.push_back(allWaveScripts.back());

        // wave 20: DOUBLE BOSS WAVE
        allWaveScripts.push_back({
            {EnemyType::ANTUMBRA, 0.1f}, // First Boss
            {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 0.1f}, {EnemyType::LOCUS, 0.2f}, {EnemyType::POLY, 10.0f}, // Minion wave
            {EnemyType::ANTUMBRA, 0.1f} // Second Boss
        });

        state = State::WAITING_FOR_PLAYER; // Start waiting for the player
        currentWaveIndex = 0;
        currentCommandIndex = 0;
        spawnTimer = 0;
    }

    void StartNextWave()
    {
        if (state == State::WAITING_FOR_PLAYER && !IsFinished())
        {
            state = State::SPAWNING;
            currentCommandIndex = 0;
            spawnTimer = 0;
        }
    }

    void Update(float deltaTime, vector<unique_ptr<Entity>> &entities, int activeEnemies)
    {
        if (state == State::FINISHED)
            return;

        if (state == State::SPAWNING)
        {
            spawnTimer -= deltaTime;
            if (spawnTimer <= 0)
            {
                vector<SpawnCommand> &script = allWaveScripts[currentWaveIndex];

                SpawnCommand &command = script[currentCommandIndex];

                int waveNum = currentWaveIndex + 1;
                switch (command.type)
                {
                case EnemyType::FLARE:
                    entities.push_back(make_unique<flare_enemy>());
                    break;
                case EnemyType::MONO:
                    entities.push_back(make_unique<mono_enemy>());
                    break;
                case EnemyType::LOCUS:
                    entities.push_back(make_unique<locus_enemy>());
                    break;
                case EnemyType::CRAWLER:
                    entities.push_back(make_unique<crawler_enemy>()); break;
                case EnemyType::POLY:
                    entities.push_back(make_unique<poly_enemy>());
                    break;
                case EnemyType::ANTUMBRA:
                    entities.push_back(make_unique<antumbra_enemy>());
                    break;
                }

                spawnTimer = command.delayUntilNext;
                currentCommandIndex++;
                if (currentCommandIndex >= script.size())
                {
                    state = State::WAVE_IN_PROGRESS; // Done spawning, now just wait
                }
            }
        }
        else if (state == State::WAVE_IN_PROGRESS)
        {
            // wait for all enemies death
            if (activeEnemies == 0)
            {
                currentWaveIndex++;

                // Check if the completed wave was a boss wave
                if (currentWaveIndex > 0 && (currentWaveIndex % BOSS_WAVE_INTERVAL == 0))
                {
                    currentStage++;
                    enemy_health_multiplier += 0.5f; // Permanently increase health of all future enemies
                }

                if (currentWaveIndex >= allWaveScripts.size())
                {
                    state = State::FINISHED; // win condition?
                }
                else
                {
                    state = State::WAITING_FOR_PLAYER;
                }
            }
        }
    }

  private:
    vector<vector<SpawnCommand>> allWaveScripts;
    int currentWaveIndex;
    int currentCommandIndex;
    int currentStage = 1;
    float spawnTimer;

    enum class State
    {
        WAITING_FOR_PLAYER,
        SPAWNING,
        WAVE_IN_PROGRESS, // Spawning done, waiting for enemies to be cleared
        FINISHED
    };
    State state;
};

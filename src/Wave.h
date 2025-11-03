#pragma once

#include "Enemy.h"
#include "Entity.h"
#include <memory>
#include <vector>

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
    int GetWaveNumber() { return currentWaveIndex + 1; }
    bool IsWaveActive() { return state == State::SPAWNING || state == State::WAVE_IN_PROGRESS; }
    bool IsFinished() { return state == State::FINISHED; }

    bool CanStartNextWave() { return state == State::WAITING_FOR_PLAYER; }

    WaveManager()
    {
        // 'allWaveScripts' is a vector of wave scripts
        // a wave script is a vector of SpawnCommands

        // Wave 1: 10 flare enemies, 1 second apart
        std::vector<SpawnCommand> wave1_script;
        for (int i = 0; i < 10; i++)
        {
            wave1_script.push_back({EnemyType::FLARE, 1.0f});
        }
        allWaveScripts.push_back(wave1_script);

        // Wave 2: 15 standard enemies, 0.5s apart
        std::vector<SpawnCommand> wave2_script;
        for (int i = 0; i < 15; i++)
        {
            wave2_script.push_back({EnemyType::FLARE, 0.5f});
        }
        allWaveScripts.push_back(wave2_script);

        // TODO : add more waves

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

    void Update(float deltaTime, std::vector<std::unique_ptr<Entity>> &entities, int activeEnemies)
    {
        if (state == State::FINISHED)
            return;

        if (state == State::SPAWNING)
        {
            spawnTimer -= deltaTime;
            if (spawnTimer <= 0)
            {
                std::vector<SpawnCommand> &script = allWaveScripts[currentWaveIndex];
                SpawnCommand &command = script[currentCommandIndex];

                int waveNum = currentWaveIndex + 1;
                switch (command.type)
                {
                case EnemyType::FLARE:
                    entities.push_back(std::make_unique<flare_enemy>());
                    break;
                case EnemyType::MONO:
                    entities.push_back(std::make_unique<mono_enemy>());
                    break;
                case EnemyType::LOCUS:
                    entities.push_back(std::make_unique<locus_enemy>());
                    break;
                case EnemyType::CRAWLER:
                    entities.push_back(std::make_unique<crawler_enemy>());
                case EnemyType::POLY:
                    entities.push_back(std::make_unique<poly_enemy>());
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
    std::vector<std::vector<SpawnCommand>> allWaveScripts;
    int currentWaveIndex;
    int currentCommandIndex;
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

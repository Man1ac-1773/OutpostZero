#pragma once

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
    int GetWaveNumber() { return currentWaveIndex + 1; }
    bool IsWaveActive() { return state == State::SPAWNING || state == State::WAVE_IN_PROGRESS; }
    bool IsFinished() { return state == State::FINISHED; }

    bool CanStartNextWave() { return state == State::WAITING_FOR_PLAYER; }

    WaveManager()
    {
        // 'allWaveScripts' is a vector of wave scripts
        // a wave script is a vector of SpawnCommands

        // wave 1: 10 flare enemies, 1 second apart
        vector<SpawnCommand> wave1;
        for (int i = 0; i < 10; i++)
        {
            wave1.push_back({EnemyType::FLARE, 0.5f});
        }
        allWaveScripts.push_back(wave1);

        // wave 2: 15 standard enemies, 0.5s apart
        vector<SpawnCommand> wave2;
        for (int i = 0; i < 15; i++)
        {
            wave2.push_back({EnemyType::FLARE, 0.25f});
        }
        allWaveScripts.push_back(wave2);

        // wave 3 : 15 standard, 0.1s apart
        vector<SpawnCommand> wave3;
        for (int i = 0; i < 15; i++)
        {
            wave3.push_back({EnemyType::FLARE, 0.1f});
        }
        allWaveScripts.push_back(wave3);

        vector<SpawnCommand> wave4;
        for (int i = 0; i < 5; i++)
        {
            wave4.push_back({EnemyType::MONO, 0.5f});
        }
        allWaveScripts.push_back(wave4);
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
                    entities.push_back(make_unique<crawler_enemy>());
                case EnemyType::POLY:
                    entities.push_back(make_unique<poly_enemy>());
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
    vector<vector<SpawnCommand>> allWaveScripts;
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

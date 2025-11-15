#pragma once

#include "Types.h"
#include <map>
using namespace std; 

class StatsManager
{
  public:
    map<EnemyType, int> enemies_killed_by_type;
    int total_money_spent = 0;
    unsigned long long total_damage_done = 0;
    StatsManager() {}
    void reset()
    {
        enemies_killed_by_type.clear();
        total_money_spent = 0;
        total_damage_done = 0;
    }

    void EnemyKilled(EnemyType type)
    {
        enemies_killed_by_type[type]++;
    }

    void MoneySpent(int amount)
    {
        total_money_spent += amount;
    }

    void DamageDealt(float amount)
    {
        total_damage_done += amount;
    }

    
};
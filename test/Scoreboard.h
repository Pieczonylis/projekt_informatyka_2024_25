#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <vector>
#include <string>

struct PlayerData
{
    std::string nick;
    int score = 0;
};

bool saveScoreboard(const std::vector<PlayerData>& players);
bool loadScoreboard(std::vector<PlayerData>& players);

#endif 

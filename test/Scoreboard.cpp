#include "Scoreboard.h"
#include <fstream>
#include <iostream>
#include <algorithm>


bool saveScoreboard(const std::vector<PlayerData>& players)
{
    std::ofstream file("scoreboard.txt");
    if (!file.is_open())
    {
        std::cerr << "Nie mozna otworzyc scoreboard.txt do zapisu!\n";
        return false;
    }
        file << players.size() << "\n";
    for (auto& p : players)
    {
        file << p.nick << " " << p.score << "\n";
    }
    file.close();
    return true;
}
bool loadScoreboard(std::vector<PlayerData>& players)
{
    std::ifstream file("scoreboard.txt");
    if (!file.is_open())
    {
        std::cerr << "Brak pliku scoreboard.txt lub nie mozna go otworzyc.\n";
        return false;
    }

    size_t count;
    file >> count;
    players.clear();
    players.reserve(count);

    for (size_t i = 0; i < count; i++)
    {
        PlayerData pd;
        file >> pd.nick >> pd.score;
        players.push_back(pd);
    }

    file.close();
    //sortowañsko
    std::sort(players.begin(), players.end(),
        [](const PlayerData& a, const PlayerData& b) { return a.score > b.score; }
    );

    return true;
}

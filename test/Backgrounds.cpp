#include "Backgrounds.h"
#include <iostream>

BackgroundManager::BackgroundManager()
{
    if (!bgSpace1.loadFromFile("Resources/bgSpace1.png"))
        std::cerr << "Blad ladowania bgSpace1.png\n";
    if (!bgSpace2.loadFromFile("Resources/bgSpace2.png"))
        std::cerr << "Blad ladowania bgSpace2.png\n";

    if (!bgNormal1.loadFromFile("Resources/bg2.png"))
        std::cerr << "Blad ladowania bg2.png\n";
    if (!bgNormal2.loadFromFile("Resources/bg2b.png"))
        std::cerr << "Blad ladowania bg2b.png\n";

    if (!bgWinter1.loadFromFile("Resources/bgWinter1.png"))
        std::cerr << "Blad ladowania bgWinter1.png\n";
    if (!bgWinter2.loadFromFile("Resources/bgWinter2.png"))
        std::cerr << "Blad ladowania bgWinter2.png\n";

    if (!bgForest1.loadFromFile("Resources/bgForest1.png"))
        std::cerr << "Blad ladowania bgForest1.png\n";
    if (!bgForest2.loadFromFile("Resources/bgForest2.png"))
        std::cerr << "Blad ladowania bgForest2.png\n";
}

sf::Texture& BackgroundManager::getBackgroundTexture(int level)
{
    switch (level)
    {
    case 1:  return bgSpace1;
    case 2:  return bgSpace2;
    case 3:  return bgNormal1;
    case 4:  return bgNormal2;
    case 5:  return bgWinter1;
    case 6:  return bgWinter2;
    case 7:  return bgForest1;
    case 8:  return bgForest2;
    case 9:
    case 10:
        return bgForest2;
    default:
        return bgNormal1;
    }
}

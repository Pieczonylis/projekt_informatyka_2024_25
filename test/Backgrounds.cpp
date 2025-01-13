#include "Backgrounds.h"
#include <iostream>

BackgroundManager::BackgroundManager()
{
    // Wczytujemy ró¿ne t³a z plików:
    if (!bgSpace1.loadFromFile("Resources/bgSpace1.png"))
        std::cerr << "Blad ladowania Resources/bgSpace1.png\n";
    if (!bgSpace2.loadFromFile("Resources/bgSpace2.png"))
        std::cerr << "Blad ladowania Resources/bgSpace2.png\n";

    if (!bgNormal1.loadFromFile("Resources/bg2.png"))
        std::cerr << "Blad ladowania Resources/bg2.png\n";
    if (!bgNormal2.loadFromFile("Resources/bg2b.png"))
        std::cerr << "Blad ladowania Resources/bg2b.png\n";

    if (!bgWinter1.loadFromFile("Resources/bgWinter1.png"))
        std::cerr << "Blad ladowania Resources/bgWinter1.png\n";
    if (!bgWinter2.loadFromFile("Resources/bgWinter2.png"))
        std::cerr << "Blad ladowania Resources/bgWinter2.png\n";

    // Leœne mapy
    if (!bgForest1.loadFromFile("Resources/bgForest1.png"))
        std::cerr << "Blad ladowania Resources/bgForest1.png\n";
    if (!bgForest2.loadFromFile("Resources/bgForest2.png"))
        std::cerr << "Blad ladowania Resources/bgForest2.png\n";
}

/**
 * Zwraca jedn¹ z wczytanych tekstur w zale¿noœci od poziomu.
 */
sf::Texture& BackgroundManager::getBackgroundTexture(int level)
{
    // poziomy
    switch (level)
    {
    case 1:
        return bgSpace1;    // kosmos 1
    case 2:
        return bgSpace2;    // kosmos 2
    case 3:
        return bgNormal1;   // normalne 1
    case 4:
        return bgNormal2;   // normalne 2
    case 5:
        return bgWinter1;   // zimowe 1
    case 6:
        return bgWinter2;   // zimowe 2
    case 7:
        return bgForest1;   // las 1
    case 8:
        return bgForest2;   // las 2
    case 9:
    case 10:
        return bgForest2;
    default:
        return bgForest2;
    }
}

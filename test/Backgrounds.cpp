#include "Backgrounds.h"
#include <iostream>

BackgroundManager::BackgroundManager()
{
    // Wczytajmy przyk³adowe pliki:
    // Poziomy 1 i 2 -> "kosmos"
    if (!bgSpace1.loadFromFile("Resources/bgSpace1.png"))
        std::cerr << "Blad ladowania Resources/bgSpace1.png\n";
    if (!bgSpace2.loadFromFile("Resources/bgSpace2.png"))
        std::cerr << "Blad ladowania Resources/bgSpace2.png\n";

    // Dalsze t³a
    if (!bg2Texture.loadFromFile("Resources/bg2.png"))
        std::cerr << "Blad ladowania Resources/bg2.png\n";
    if (!bgWinter1.loadFromFile("Resources/bgWinter1.png"))
        std::cerr << "Blad ladowania Resources/bgWinter1.png\n";
    if (!bgWinter2.loadFromFile("Resources/bgWinter2.png"))
        std::cerr << "Blad ladowania Resources/bgWinter2.png\n";
}

/**
 * Zwraca jedn¹ z wczytanych tekstur zale¿nie od numeru poziomu.
 * U¿ywamy switch-case.
 */
sf::Texture& BackgroundManager::getBackgroundTexture(int level)
{
    switch (level)
    {
    case 1: // kosmos 1
        return bgSpace1;
    case 2: // kosmos 2
        return bgSpace2;
    case 3:
    case 4:
        return bg2Texture;
    case 5:
    case 6:
        return bgWinter1;
    case 7:
    case 8:
        return bgWinter2;
    default:
        // jeœli np. 9,10, itp. – na razie zwracamy cokolwiek
        return bg2Texture;
    }
}

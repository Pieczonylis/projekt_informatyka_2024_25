#pragma once
#ifndef BACKGROUNDS_H
#define BACKGROUNDS_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

/**
 * Prosta struktura do „ruchomych” dekoracji t³a (psy, chmury, itp.)
 */
struct MovingSprite
{
    sf::Sprite sprite;
    sf::Vector2f velocity;
};

/**
 * Klasa ³aduj¹ca i zwracaj¹ca ró¿ne t³a w zale¿noœci od numeru poziomu.
 * Dodajemy te¿ "kosmiczne" t³a dla poziomów 1 i 2.
 */
class BackgroundManager
{
private:
    // Tekstury do ró¿nych poziomów:
    sf::Texture bgSpace1;     // kosmos np. dla poziomu 1
    sf::Texture bgSpace2;     // kosmos np. dla poziomu 2
    sf::Texture bg2Texture;   // np. dla poziomów 3-4
    sf::Texture bgWinter1;    // 5-6
    sf::Texture bgWinter2;    // 7-8
    // itd. – mo¿na dodaæ kolejne, jeœli potrzebne

public:
    BackgroundManager();
    sf::Texture& getBackgroundTexture(int level);
};

#endif // BACKGROUNDS_H

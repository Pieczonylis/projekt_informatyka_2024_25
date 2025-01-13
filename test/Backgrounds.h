#pragma once
#ifndef BACKGROUNDS_H
#define BACKGROUNDS_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

/**
 * Prosta struktura do �ruchomych� dekoracji t�a (psy, chmury, itp.)
 */
struct MovingSprite
{
    sf::Sprite sprite;
    sf::Vector2f velocity;
};

/**
 * Klasa �aduj�ca i zwracaj�ca r�ne t�a w zale�no�ci od numeru poziomu.
 * Dodajemy te� "kosmiczne" t�a dla poziom�w 1 i 2.
 */
class BackgroundManager
{
private:
    // Tekstury do r�nych poziom�w:
    sf::Texture bgSpace1;     // kosmos np. dla poziomu 1
    sf::Texture bgSpace2;     // kosmos np. dla poziomu 2
    sf::Texture bg2Texture;   // np. dla poziom�w 3-4
    sf::Texture bgWinter1;    // 5-6
    sf::Texture bgWinter2;    // 7-8
    // itd. � mo�na doda� kolejne, je�li potrzebne

public:
    BackgroundManager();
    sf::Texture& getBackgroundTexture(int level);
};

#endif // BACKGROUNDS_H

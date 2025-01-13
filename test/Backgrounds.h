#ifndef BACKGROUNDS_H
#define BACKGROUNDS_H

#include <SFML/Graphics.hpp>

/**
 * Struktura do �ruchomych� dekoracji (psy, chmury, itp.).
 */
struct MovingSprite
{
    sf::Sprite sprite;
    sf::Vector2f velocity;
};

/**
 * Klasa �aduj�ca i zwracaj�ca r�ne t�a w zale�no�ci od poziomu.
 * Dodali�my:
 *  - kosmos (1�2)
 *  - "normalne" (3�4)
 *  - zimowe (5�6)
 *  - le�ne (7�8)
 *  - ewentualnie 9�10 � zdefiniujmy jako le�ne2
 */
class BackgroundManager
{
private:
    sf::Texture bgSpace1;
    sf::Texture bgSpace2;
    sf::Texture bgNormal1;
    sf::Texture bgNormal2;
    sf::Texture bgWinter1;
    sf::Texture bgWinter2;
    sf::Texture bgForest1;
    sf::Texture bgForest2;

public:
    BackgroundManager();
    sf::Texture& getBackgroundTexture(int level);
};

#endif // BACKGROUNDS_H

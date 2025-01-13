#ifndef ENTITIES_H
#define ENTITIES_H

#include <SFML/Graphics.hpp>

static const unsigned WINDOW_WIDTH = 1920;
static const unsigned WINDOW_HEIGHT = 1080;

/**
 * Klasa bazowa dla obiektów, które aktualizuj¹ i rysuj¹ siê na ekranie.
 */
class GameObject
{
public:
    virtual ~GameObject() {}
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};

// ---------------------------
// Klasa pi³ki
// ---------------------------
class Ball : public GameObject
{
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;

public:
    Ball(float radius = 12.f);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

    sf::FloatRect getGlobalBounds() const;
    sf::Vector2f& getVelocity();
    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;
    float getRadius() const;
};

// ---------------------------
// Klasa paletki
// ---------------------------
class Paddle : public GameObject
{
private:
    sf::RectangleShape shape;
    float speed;
public:
    Paddle(sf::Vector2f size = sf::Vector2f(120.f, 30.f));

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getGlobalBounds() const;
};

// ---------------------------
// Bazowa klasa celu
// ---------------------------
class Target : public GameObject
{
protected:
    bool destroyed = false;
public:
    virtual sf::FloatRect getGlobalBounds() const = 0;
    bool isDestroyed() const { return destroyed; }
    void destroy() { destroyed = true; }
};

// ---------------------------
// Konkrety - ró¿ne kszta³ty
// ---------------------------
class IrregularBlock : public Target
{
private:
    sf::ConvexShape shape;
public:
    IrregularBlock(sf::Vector2f position);
    void update(float deltaTime) override {}
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getGlobalBounds() const override;
};

class RectBlock : public Target
{
private:
    sf::RectangleShape shape;
public:
    RectBlock(sf::Vector2f position);
    void update(float) override {}
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getGlobalBounds() const override;
};

class CircleTarget : public Target
{
private:
    sf::CircleShape shape;
public:
    CircleTarget(sf::Vector2f position, float radius);
    void update(float) override {}
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getGlobalBounds() const override;
};

class BigRectBlock : public Target
{
private:
    sf::RectangleShape shape;
public:
    BigRectBlock(sf::Vector2f position);
    void update(float) override {}
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getGlobalBounds() const override;
};

#endif // ENTITIES_H

#include "Entities.h"
#include <cmath> // std::abs

// ---------------------------
// Ball
// ---------------------------
Ball::Ball(float radius)
{
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Yellow);
    shape.setOrigin(radius, radius);
    shape.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
    velocity = sf::Vector2f(300.f, 300.f);
}

void Ball::update(float deltaTime)
{
    sf::Vector2f pos = shape.getPosition();
    pos += velocity * deltaTime;
    shape.setPosition(pos);

    // Odbicie od lewej/prawej krawêdzi
    if (pos.x - shape.getRadius() < 0.f)
    {
        shape.setPosition(shape.getRadius(), pos.y);
        velocity.x = -velocity.x;
    }
    else if (pos.x + shape.getRadius() > WINDOW_WIDTH)
    {
        shape.setPosition(WINDOW_WIDTH - shape.getRadius(), pos.y);
        velocity.x = -velocity.x;
    }

    // Odbicie od górnej krawêdzi
    if (pos.y - shape.getRadius() < 0.f)
    {
        shape.setPosition(pos.x, shape.getRadius());
        velocity.y = -velocity.y;
    }
}

void Ball::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}

sf::FloatRect Ball::getGlobalBounds() const
{
    return shape.getGlobalBounds();
}

sf::Vector2f& Ball::getVelocity()
{
    return velocity;
}

void Ball::setPosition(float x, float y)
{
    shape.setPosition(x, y);
}

sf::Vector2f Ball::getPosition() const
{
    return shape.getPosition();
}

float Ball::getRadius() const
{
    return shape.getRadius();
}

// ---------------------------
// Paddle
// ---------------------------
Paddle::Paddle(sf::Vector2f size)
{
    shape.setSize(size);
    shape.setFillColor(sf::Color::Cyan);
    shape.setOrigin(size.x / 2.f, size.y / 2.f);
    shape.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 80.f);
    speed = 600.f;
}

void Paddle::update(float deltaTime)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        shape.move(-speed * deltaTime, 0.f);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        shape.move(speed * deltaTime, 0.f);
    }

    // Ograniczenie ruchu
    if (shape.getPosition().x - shape.getSize().x / 2.f < 0.f)
    {
        shape.setPosition(shape.getSize().x / 2.f, shape.getPosition().y);
    }
    else if (shape.getPosition().x + shape.getSize().x / 2.f > WINDOW_WIDTH)
    {
        shape.setPosition(WINDOW_WIDTH - shape.getSize().x / 2.f, shape.getPosition().y);
    }
}

void Paddle::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}

sf::FloatRect Paddle::getGlobalBounds() const
{
    return shape.getGlobalBounds();
}

// ---------------------------
// IrregularBlock
// ---------------------------
IrregularBlock::IrregularBlock(sf::Vector2f position)
{
    shape.setPointCount(5);
    shape.setPoint(0, sf::Vector2f(0.f, 0.f));
    shape.setPoint(1, sf::Vector2f(20.f, 0.f));
    shape.setPoint(2, sf::Vector2f(70.f, 20.f));
    shape.setPoint(3, sf::Vector2f(50.f, 40.f));
    shape.setPoint(4, sf::Vector2f(0.f, 30.f));
    shape.setFillColor(sf::Color::Magenta);
    shape.setPosition(position);
}

void IrregularBlock::draw(sf::RenderWindow& window)
{
    if (!destroyed) window.draw(shape);
}

sf::FloatRect IrregularBlock::getGlobalBounds() const
{
    return shape.getGlobalBounds();
}

// ---------------------------
// RectBlock
// ---------------------------
RectBlock::RectBlock(sf::Vector2f position)
{
    shape.setSize(sf::Vector2f(60.f, 30.f));
    shape.setFillColor(sf::Color::White);
    shape.setPosition(position);
}

void RectBlock::draw(sf::RenderWindow& window)
{
    if (!destroyed) window.draw(shape);
}

sf::FloatRect RectBlock::getGlobalBounds() const
{
    return shape.getGlobalBounds();
}

// ---------------------------
// CircleTarget
// ---------------------------
CircleTarget::CircleTarget(sf::Vector2f position, float radius)
{
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin(radius, radius);
    shape.setPosition(position);
}

void CircleTarget::draw(sf::RenderWindow& window)
{
    if (!destroyed) window.draw(shape);
}

sf::FloatRect CircleTarget::getGlobalBounds() const
{
    return shape.getGlobalBounds();
}

// ---------------------------
// BigRectBlock
// ---------------------------
BigRectBlock::BigRectBlock(sf::Vector2f position)
{
    shape.setSize(sf::Vector2f(80.f, 40.f));
    shape.setFillColor(sf::Color::Magenta);
    shape.setPosition(position);
}

void BigRectBlock::draw(sf::RenderWindow& window)
{
    if (!destroyed) window.draw(shape);
}

sf::FloatRect BigRectBlock::getGlobalBounds() const
{
    return shape.getGlobalBounds();
}

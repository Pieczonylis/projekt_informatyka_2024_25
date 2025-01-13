#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

#include "Scoreboard.h"
#include "Entities.h"
#include "Backgrounds.h"

enum class GameState
{
    MENU,
    ENTER_NAME,
    PLAY,
    TUTORIAL,
    SCOREBOARD,
    EXIT_CONFIRM,
    GAME_OVER,
    LEVEL_COMPLETE,
    EXIT
};

class Game
{
private:
    sf::RenderWindow window;
    //bool roboczy do nicków
    bool nicknameEntered;

    // Obiekty gry
    Ball ball;
    Paddle paddle;
    std::vector<std::unique_ptr<Target>> blocks;

    // T³a
    BackgroundManager bgManager;
    sf::Sprite backgroundSprite;
    sf::Texture menuBackgroundTexture;
    sf::Sprite  menuBackgroundSprite;

    // T³o scoreboardu
    sf::Texture scoreBackgroundTexture;
    sf::Sprite  scoreBackgroundSprite;

    // Animowany sprite w menu
    sf::Texture menuAnimTexture;
    sf::Sprite  menuAnimSprite;
    sf::Vector2f menuAnimVelocity;

    // Tekstury do sprite'ów (chmura, drzewo)
    sf::Texture cloudTexture;
    sf::Texture treeTexture;

    // Dekoracje
    std::vector<sf::Sprite> staticSprites;
    std::vector<MovingSprite> movingSprites;

    // Font i teksty
    sf::Font font;
    sf::Text textScore;
    sf::Text textLevel;
    sf::Text textTutorial;
    sf::Text textMenu;
    sf::Text textGameOver;
    sf::Text textLevelComplete;
    sf::Text textScoreboard;
    sf::Text textEnterName;

    // Stan wprowadzania nicku
    std::string currentNickname;

    // Stan gry
    GameState currentState;

    // Dane gry
    int score;
    int level;

    // Tablica wyników
    std::vector<PlayerData> players;

    // Zegar
    sf::Clock clock;

public:
    Game();
    void run();

private:
    void handleEvents();
    void handleEnterNameEvent(const sf::Event& event);
    void update(float deltaTime);
    void render();
    void loadLevel(int lvl);

    bool isTypeOneWorld(int lvl) const;
};

#endif // GAME_H
